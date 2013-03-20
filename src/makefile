-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: gnm pyModule

# Tool invocations
gnm: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc gnm.c -o "gnm" -I../include
	@echo 'Finished building target: $@'
	@echo ' '

pyModule:
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc python_binding.c -o "myextension.so" -fPIC -shared -I/usr/include/python3.2 -I../include
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) gnm
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
