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
USEMODULE += fmt
USEMODULE += xtimer
USEMODULE += xtimer_no_ztimer_default

# Shell
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
# end shell

# driver sx127x
USEMODULE += od
USEMODULE += sx1276
# end driver sx127x

# semtech-lorawan ------------------------------
DEVEUI ?= 70B3D57ED005D1E7
APPEUI ?= 0000000000000000
APPKEY ?= 272E779C396D1E51E94B9BDD39558EB0

LORA_DRIVER ?= sx1276
LORA_REGION ?= EU868

USEPKG += semtech-loramac

USEMODULE += semtech_loramac_rx

USEMODULE += auto_init_loramac

# Default IotLab Config to run the test
ifneq (,$(filter iotlab%,$(MAKECMDGOALS)))
  IOTLAB_NODES ?= 1
  IOTLAB_TYPE  ?= st-lrwan1:sx1276
  IOTLAB_SITE  ?= saclay
  include $(RIOTBASE)/dist/testbed-support/Makefile.iotlab
endif
# --------------------------------------

# modules directory --------------------
USEMODULE += modules
INCLUDES += -I$(CURDIR)/include
DIRS += $(CURDIR)/modules
# ----------------------------------------

include $(RIOTBASE)/Makefile.include