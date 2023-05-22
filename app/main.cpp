#include "../include/gpbConfigManager.hpp"
#include "../include/gpbExceptions.hpp"
#include "../include/gpbWindowManager.hpp"
#include "../include/gpbMessage.hpp"
#include "../include/gpbRendererManager.hpp"
#include "../include/gpbUIPresenter.hpp"
#include "../include/gpbUIInteractionEventsHandler.hpp"
#include "../include/gpbActors.hpp"

void loadConfig()
{
	try
	{
		gpbCMM::reloadConfiguration();
	}
	catch (gpbNoConfigFileError)
	{
		gpbCMM::updateConfiguration();
	}
}
gpbWindowPtr createWindow()
{
	return gpbWMM::open(gpbCMM::configBuffer.videoMode);
}
void openRenderer()
{
	try
	{
		gpbRMM::open(gpbCMM::configBuffer.API, gpbWMM::getNativeWindowPointer(), gpbVECTOR2(1024, 768));
	}
	catch (gpbOpenGLUnsupported)
	{
		gpbMSG::sendMessage("OPENGL UNSUPPORTED", "OpenGL 4.6 unsupported.\nApplication will exit.", gpbExit::EXIT);
	}
	catch (gpbD3D11Unsupported)
	{
		gpbMSG::sendMessage("D3D11 UNSUPPORTED", "D3D11 unsupported.\nApplication will exit.", gpbExit::EXIT);
	}
	catch (gpbVulkanUnsupported)
	{
		gpbMSG::sendMessage("VULKAN UNSUPPORTED", "Vulkan 1.2 unsupported.\nApplication will exit.", gpbExit::EXIT);
	}
	catch (gpbD3D12Unsupported)
	{
		gpbMSG::sendMessage("D3D12 UNSUPPORTED", "D3D12 unsupported.\nApplication will exit.", gpbExit::EXIT);
	}
}

static gpbUINT32 UIPage{};

int main()
{
	// Initialize app.
	{
		loadConfig();
		auto reg = createWindow();
		openRenderer();
		auto buttons = gpbUIInteractionEventsHandler::open(reg);
		gpbUIPresenter::loadMenuUI(buttons);
	}

	gpbRMM::setRenderTargetViewClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	gpbWMM::show();
	while (!gpbWMM::shouldClose())
	{
		gpbUIPresenter::presentUI(UIPage);

		gpbRMM::present();

		auto event = gpbUIInteractionEventsHandler::interact(UIPage);
		if (event == gpbBenchmarkEvent::LOAD_BENCHMARK)
		{
			gpbActors::loadActors();
		}
	}

	gpbRMM::close();
	gpbWMM::close();
	return 0;
}