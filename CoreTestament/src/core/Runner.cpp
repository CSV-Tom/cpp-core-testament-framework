#include "Testament/Runner.hpp"

#include "Internal/InternalRegistry.hpp"

#include <iostream>




#include "EventSystem/EventManager.hpp"
#include "EventSystem/ServiceLocator.hpp"
#include "EventHandlers/LogEventHandler.hpp"



namespace Testament {

int Runner::run(int argc, char** argv)
{

    EventManager eventManager;
    ServiceLocator::provide(&eventManager); // Bereitstellen der Instanz
    LogEventHandler handler(ServiceLocator::getEventManager().getDispatcher());
    ServiceLocator::getEventManager().getDispatcher().dispatch(LogEvent("Hello from ServiceLocator!"));


    auto& registry = InternalRegistry::getInstance();
    for (auto& suite : registry.getAllSuites()) {
        std::cout << suite->getName() << std::endl;
        suite->run();
    }
    //TODO:
    //
    // TestRegistry::instance().run(argc, argv);
    return argc > 0 && argv != nullptr;
}

}