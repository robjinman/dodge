ifeq ($(PLATFORM), linux)
	include $(BASE_DIR)/renderer/ogles2.0/Makefile.inc
else
	ifeq ($(PLATFORM), windows)
		include $(BASE_DIR)/renderer/ogles2.0/Makefile.inc
	endif
endif