#include "main.h"

int main(int argc, char *argv[])
{
	sys = new System(getProgramName(argv[0]));
	Logger logger("Main");
	logger.info("Program started.");

	useconds_t mainLoopInterval = sys->getConfigValue<useconds_t>("global", "main_loop_interval");
	unsigned int logoTime = sys->getConfigValue<unsigned int>("global", "logo_screen_time");

	// Add smart pointer factories to the vector
	processFactories.push_back([]()
							   { return std::make_shared<TempGauge>(); });
	processFactories.push_back([]()
							   { return std::make_shared<FlowSensor>(); });
	// processFactories.push_back([]()
	// 						   { return std::make_shared<GPS>(); });

	DigitalGauge digitalGauge;
	AnalogConverter analogConverter;
	CoolantTempSensor coolantTempSensor;

	digitalGauge.showLogo(logoTime);
	digitalGauge.setScreen(DIGITAL_GAUGE);

	logger.info("Setting up shared memory for the engine readings.");
	engineValues = (EngineValues *)mmap(
		NULL,
		sizeof(EngineValues),
		PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_ANONYMOUS,
		-1,
		0);

	if (engineValues == MAP_FAILED || engineValues == NULL)
	{
		logger.error("mmap failed for engine readings or returned NULL pointer.");
		exit(1);
	}

	// Iterate and instantiate processes during iteration
	for (const auto &factory : processFactories)
	{
		pid_t pid = fork();

		if (pid < 0)
		{
			logger.error("Fork failed!!");
		}
		else if (pid == 0)
		{
			// Instantiate here
			std::shared_ptr<Process> process = factory();
			process->loop(engineValues);
			exit(0);
		}
		else
		{
			// Track the child PID and description
			ChildProcess childProcess = {pid, "Sarasa"};
			childProcesses.push_back(childProcess);
		}
	}

	// Exception handling: ctrl + c
	signal(SIGINT, signalHandler);

	logger.info("Entering main loop.");
	// ### MAIN LOOP ###
	while (!terminateProgram)
	{
		engineValues->temp.store(coolantTempSensor.readTemp());
		engineValues->volts.store(analogConverter.getVolts());

		digitalGauge.draw(engineValues);

		if (engineValues->volts < 6)
		{
			engineValues->ignition = false;
		}
		else
		{
			engineValues->ignition = true;
		}

		usleep(mainLoopInterval);
	}

	logger.info("Exiting main loop. Cleaning up resources.");

	digitalGauge.setScreen(TORINO_LOGO);
	digitalGauge.showLogo(logoTime);

	terminateChildProcesses(childProcesses);

	logger.info("Exiting...");
	sys->shutdown();

	logger.info("Program terminated.");
}
