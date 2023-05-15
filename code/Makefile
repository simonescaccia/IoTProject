# name of your application
APPLICATION = irrigation-water-leakage-detection

# If no BOARD is found in the environment, use this default:
BOARD ?= b-l072z-lrwan1

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Timer
USEMODULE += xtimer

# Shell
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
# end shell

# driver sx127x
USEMODULE += od
USEMODULE += sx1276
# end driver sx127x

# lorawan ------------------------------
DEVEUI ?= 70B3D57ED005D1E7
APPEUI ?= 0000000000000000
APPKEY ?= 272E779C396D1E51E94B9BDD39558EB0
# OTAA compile time configuration keys
CFLAGS += -DCONFIG_LORAMAC_APP_KEY_DEFAULT=\"$(APPKEY)\"
CFLAGS += -DCONFIG_LORAMAC_APP_EUI_DEFAULT=\"$(APPEUI)\"
CFLAGS += -DCONFIG_LORAMAC_DEV_EUI_DEFAULT=\"$(DEVEUI)\"

# Pass these enviroment variables to docker
DOCKER_ENV_VARS += DEVEUI
DOCKER_ENV_VARS += APPEUI
DOCKER_ENV_VARS += APPKEY

# Send a message every 20s after joining the network
SEND_PERIOD_S ?= 20

CFLAGS += -DSEND_PERIOD_S=$(SEND_PERIOD_S)

# Default region is Europe and default band is 868MHz
LORA_REGION ?= EU868

# Include the Semtech-loramac package
USEPKG += semtech-loramac
USEMODULE += auto_init_loramac

USEMODULE += fmt
FEATURES_OPTIONAL += periph_rtc

# Default IotLab Config to run the test
ifneq (,$(filter iotlab%,$(MAKECMDGOALS)))
  IOTLAB_NODES ?= 1
  IOTLAB_TYPE  ?= st-lrwan1:sx1276
  IOTLAB_SITE  ?= saclay
  include $(RIOTBASE)/dist/testbed-support/Makefile.iotlab
endif

ifneq (,$(filter test,$(MAKECMDGOALS)))
  DEFAULT_MODULE += test_utils_interactive_sync
endif
# end lorawan ----------------------------

USEMODULE += modules
INCLUDES += -I$(CURDIR)/include
DIRS += $(CURDIR)/modules

include $(RIOTBASE)/Makefile.include