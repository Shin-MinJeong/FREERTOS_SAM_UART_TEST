/*
 * bsp_touch.c
 */

#include "main.h"

#define TAG "TOUCH"

/** CHG(인터럽트) */
#define TOUCH_CHG_PIN IOPORT_CREATE_PIN(PIOD, 28)

typedef struct {
	bool     ready;            
	bool     pressed;          
	uint16_t x;
	uint16_t y;
	bool     deferred_release; 
	bool     deferred_press;   
	uint16_t deferred_x;
	uint16_t deferred_y;
	uint32_t last_event_tick;  
	uint8_t  report_id;        
} touch_state_t;

static struct mxt_device s_device;
static touch_state_t     s_state;


static uint8_t resolve_t9_report_id(struct mxt_device *device)
{
	uint8_t id = 1;
	uint8_t i;

	for (i = 0; i < device->info_object->obj_count; i++) {
		const struct mxt_object *object = &device->object_list[i];

		if (object->num_report_ids == 0) {
			continue;
		}
		if (object->type == MXT_TOUCH_MULTITOUCHSCREEN_T9) {
			return id;
		}
		id += object->num_report_ids * (object->instances + 1);
	}

	return 0;
}

static bool wait_backup_complete(struct mxt_device *device, uint16_t t6_base)
{
	uint16_t attempt;

	for (attempt = 0; attempt < BSP_TOUCH_BACKUP_POLL_MAX; attempt++) {
		uint8_t value = 0xFF;

		delay_ms(BSP_TOUCH_BACKUP_POLL_STEP_MS);
		mxt_read_config_reg(device,
				t6_base + MXT_GEN_COMMANDPROCESSOR_BACKUPNV, &value);

		if (value == 0x00) {
			LOG_I(TAG, "BACKUPNV done in %ums",
					(unsigned)((attempt + 1) * BSP_TOUCH_BACKUP_POLL_STEP_MS));
			return true;
		}
	}

	LOG_W(TAG, "BACKUPNV did not clear");
	return false;
}

static void write_configuration(struct mxt_device *device)
{
	uint8_t t8_values[]  = BSP_TOUCH_T8_VALUES;
	uint8_t t9_values[]  = BSP_TOUCH_T9_VALUES;
	uint8_t t46_values[] = BSP_TOUCH_T46_VALUES;
	uint8_t t56_values[] = BSP_TOUCH_T56_VALUES;

	const uint16_t t6_base = mxt_get_object_address(device, MXT_GEN_COMMANDPROCESSOR_T6, 0);
	const uint16_t t7_base = mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0);

	mxt_write_config_reg(device, t6_base + MXT_GEN_COMMANDPROCESSOR_RESET, 0x01);
	delay_ms(MXT_RESET_TIME);

	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_SPT_COMMSCONFIG_T18, 0), BSP_TOUCH_T18_CTRL);

	mxt_write_config_reg(device, t7_base + 0, BSP_TOUCH_T7_IDLE_ACQ);
	mxt_write_config_reg(device, t7_base + 1, BSP_TOUCH_T7_ACTIVE_ACQ);
	mxt_write_config_reg(device, t7_base + 2, BSP_TOUCH_T7_ACTIVE2IDLE);

	mxt_write_config_object(device, mxt_get_object_address(device, MXT_GEN_ACQUISITIONCONFIG_T8, 0), t8_values);
	mxt_write_config_object(device, mxt_get_object_address(device, MXT_TOUCH_MULTITOUCHSCREEN_T9, 0), t9_values);
	mxt_write_config_object(device, mxt_get_object_address(device, MXT_SPT_CTE_CONFIGURATION_T46, 0), t46_values);
	mxt_write_config_object(device, mxt_get_object_address(device, MXT_PROCI_SHIELDLESS_T56, 0), t56_values);

	mxt_write_config_reg(device, mxt_get_object_address(device, BSP_TOUCH_OBJECT_T42, 0), BSP_TOUCH_T42_CTRL);
	mxt_write_config_reg(device, t6_base + MXT_GEN_COMMANDPROCESSOR_BACKUPNV, 0x55);
	wait_backup_complete(device, t6_base);

	mxt_write_config_reg(device, t6_base + MXT_GEN_COMMANDPROCESSOR_CALIBRATE, 0x01);
	delay_ms(BSP_TOUCH_CALIBRATE_SETTLE_MS);
}

bool bsp_touch_init(void)
{
	twihs_master_options_t twi_options = {
		.speed = MXT_TWI_SPEED,
		.chip  = BSP_TOUCH_TWI_ADDRESS,
	};

	s_state.ready = false;

	bsp_board_shared_reset(BSP_TOUCH_RESET_ASSERT_MS, BSP_TOUCH_RESET_RELEASE_MS);

	ioport_set_pin_dir(TOUCH_CHG_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(TOUCH_CHG_PIN, IOPORT_MODE_PULLUP);

	if (twihs_master_setup(BSP_TOUCH_TWI_BUS, &twi_options) != STATUS_OK) {
		LOG_E(TAG, "TWI setup failed");
		return false;
	}

	if (mxt_init_device(&s_device, BSP_TOUCH_TWI_BUS, BSP_TOUCH_TWI_ADDRESS,
			TOUCH_CHG_PIN) != STATUS_OK) {
		LOG_E(TAG, "controller not responding");
		return false;
	}

	write_configuration(&s_device);

	s_state.report_id = resolve_t9_report_id(&s_device);
	if (s_state.report_id == 0) {
		LOG_E(TAG, "T9 object not found");
		return false;
	}
	s_device.multitouch_report_offset = (int8_t)s_state.report_id;
	LOG_I(TAG, "T9 report id base = %u", (unsigned)s_state.report_id);

	s_state.ready = true;
	LOG_I(TAG, "ready");
	return true;
}

static bool apply_message(const struct mxt_conf_messageprocessor_t5 *message, bool *pressed_in_this_cycle)
{
	const uint8_t  status = message->message[0];
	const uint16_t nx = ((uint16_t)message->message[1] << 4) | (message->message[3] >> 4);
	const uint16_t ny = ((uint16_t)message->message[2] << 4) | (message->message[3] & 0x0F);

	s_state.last_event_tick = xTaskGetTickCount();

	if (status & MXT_SUPPRESS_EVENT) {
		s_state.pressed = false;
		return false;
	}

	if (status & MXT_DETECT_EVENT) {
		if ((status & MXT_PRESS_EVENT) && s_state.pressed) {
			s_state.pressed        = false;
			s_state.deferred_press = true;
			s_state.deferred_x     = nx;
			s_state.deferred_y     = ny;
			return true;
		}

		s_state.x = nx;
		s_state.y = ny;
		if (!s_state.pressed) {
			*pressed_in_this_cycle = true;
		}
		s_state.pressed = true;
		return false;
	}

	if (status & MXT_RELEASE_EVENT) {
		if (*pressed_in_this_cycle) {
			s_state.deferred_release = true;
			return true;
		}
		s_state.pressed = false;
	}

	return false;
}

static bool consume_deferred_event(bsp_touch_point_t *point)
{
	if (s_state.deferred_release) {
		s_state.deferred_release = false;
		s_state.pressed          = false;

		point->x       = s_state.x;
		point->y       = s_state.y;
		point->pressed = false;
		return true;
	}

	if (s_state.deferred_press) {
		s_state.deferred_press  = false;
		s_state.x               = s_state.deferred_x;
		s_state.y               = s_state.deferred_y;
		s_state.pressed         = true;
		s_state.last_event_tick = xTaskGetTickCount();

		point->x       = s_state.x;
		point->y       = s_state.y;
		point->pressed = true;
		return true;
	}

	return false;
}

static void release_if_stuck(void)
{
	const uint32_t elapsed = xTaskGetTickCount() - s_state.last_event_tick;

	if (!s_state.pressed || elapsed <= pdMS_TO_TICKS(BSP_TOUCH_STUCK_TIMEOUT_MS)) {
		return;
	}

	s_state.pressed          = false;
	s_state.deferred_release = false;
	s_state.deferred_press   = false;

	LOG_W(TAG, "stuck press force-released");
}

void bsp_touch_poll(bsp_touch_point_t *point)
{
	struct mxt_conf_messageprocessor_t5 message;
	bool pressed_in_this_cycle = false;

	point->x       = s_state.x;
	point->y       = s_state.y;
	point->pressed = false;

	if (!s_state.ready) {
		return;
	}

	if (consume_deferred_event(point)) {
		return;
	}

	while (mxt_is_message_pending(&s_device)) {
		if (mxt_read_message(&s_device, &message) != STATUS_OK) {
			break;
		}
		if (message.reportid != s_state.report_id) {
			continue;
		}
		if (apply_message(&message, &pressed_in_this_cycle)) {
			break;
		}
	}

	release_if_stuck();

	point->x       = s_state.x;
	point->y       = s_state.y;
	point->pressed = s_state.pressed;
}
