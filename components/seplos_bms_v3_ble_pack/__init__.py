import esphome.codegen as cg
from esphome.components import seplos_bms_v3_ble, text_sensor
import esphome.config_validation as cv
import esphome.final_validate as fv
from esphome.const import CONF_ID

AUTO_LOAD = ["sensor", "text_sensor"]

CODEOWNERS = ["@syssi"]

MULTI_CONF = True

CONF_SEPLOS_BMS_V3_BLE_PACK_ID = "seplos_bms_v3_ble_pack_id"
CONF_JSON_TEXT_SENSOR = "json_text_sensor"

DEFAULT_ADDRESS = 0x00

seplos_bms_v3_ble_pack_ns = cg.esphome_ns.namespace("seplos_bms_v3_ble_pack")
SeplosBmsV3BlePack = seplos_bms_v3_ble_pack_ns.class_(
    "SeplosBmsV3BlePack", cg.Component, seplos_bms_v3_ble.SeplosBmsV3BlePackBase
)

SEPLOS_BMS_V3_BLE_PACK_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SEPLOS_BMS_V3_BLE_PACK_ID): cv.use_id(SeplosBmsV3BlePack),
    }
)


def validate_address_unique(value):
    """Validate that each address is unique per parent BMS component."""
    from esphome.const import CONF_ADDRESS
    from esphome.core import CORE

    if not hasattr(CORE, "seplos_v3_ble_pack_addresses"):
        CORE.seplos_v3_ble_pack_addresses = {}

    parent_id = value[seplos_bms_v3_ble.CONF_SEPLOS_BMS_V3_BLE_ID]
    address = value[CONF_ADDRESS]

    if parent_id not in CORE.seplos_v3_ble_pack_addresses:
        CORE.seplos_v3_ble_pack_addresses[parent_id] = set()

    if address in CORE.seplos_v3_ble_pack_addresses[parent_id]:
        raise cv.Invalid(
            f"Address 0x{address:02X} is already used by another pack component for the same parent BMS"
        )

    CORE.seplos_v3_ble_pack_addresses[parent_id].add(address)
    return value


CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SeplosBmsV3BlePack),
            cv.Optional(CONF_JSON_TEXT_SENSOR): cv.use_id(text_sensor.TextSensor),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(seplos_bms_v3_ble.seplos_bms_v3_ble_device_schema(DEFAULT_ADDRESS)),
    validate_address_unique,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await seplos_bms_v3_ble.register_seplos_bms_v3_ble_device(var, config)
    cg.add(var.set_name(config[CONF_ID].id))

    # JSON text sensor (reference to existing text sensor by ID)
    if CONF_JSON_TEXT_SENSOR in config:
        ts = await cg.get_variable(config[CONF_JSON_TEXT_SENSOR])
        cg.add(var.set_json_text_sensor(ts))


def _final_validate(config):
    full_cfg = fv.full_config.get()
    esp_cfg = full_cfg.get("esphome", {})
    pio_opts = esp_cfg.get("platformio_options", {})
    build_flags = pio_opts.get("build_flags", [])
    if isinstance(build_flags, str):
        build_flags = [build_flags]
    if any("-DWEB_VERSION" in flag for flag in build_flags) and CONF_JSON_TEXT_SENSOR not in config:
        raise cv.Invalid(
            "WEB_VERSION is defined but 'json_text_sensor' is not configured. "
            "Please add a 'json_text_sensor' reference to your Seplos BMS V3 pack configuration."
        )
    return config


FINAL_VALIDATE_SCHEMA = _final_validate
