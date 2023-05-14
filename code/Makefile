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

# driver sx127x
USEMODULE += od

include $(RIOTBASE)/Makefile.include