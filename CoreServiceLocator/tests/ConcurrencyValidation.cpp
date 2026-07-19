#include "CoreServices/ServiceLocator.hpp"

#include <atomic>
#include <barrier>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

namespace {

class Service final : public Core::Services::IService {
public:
    std::atomic_uint calls{};
};

}

int main() {
    Core::Services::ServiceLocator locator;
    auto service = std::make_shared<Service>();
    locator.registerService(service);

    std::vector<std::jthread> readers;
    for (unsigned int thread = 0; thread < 8; ++thread) {
        readers.emplace_back([&locator, &service] {
            for (unsigned int iteration = 0; iteration < 1'000; ++iteration) {
                if (locator.getService<Service>() != service) return;
                ++service->calls;
            }
        });
    }
    readers.clear();
    const bool parallelReadsSucceeded = service->calls == 8'000;

    auto consumer = locator.getService<Service>();
    std::jthread unregister([&locator] { locator.unregisterService<Service>(); });
    unregister.join();

    bool lookupRejected = false;
    try {
        static_cast<void>(locator.getService<Service>());
    } catch (const std::runtime_error&) {
        lookupRejected = true;
    }

    Core::Services::ServiceLocator registrations;
    std::atomic_uint registered{};
    std::atomic_uint duplicates{};
    std::vector<std::jthread> writers;
    for (unsigned int thread = 0; thread < 8; ++thread) {
        writers.emplace_back([&] {
            try {
                registrations.registerService(std::make_shared<Service>());
                ++registered;
            } catch (const std::runtime_error&) {
                ++duplicates;
            }
        });
    }
    writers.clear();

    Core::Services::ServiceLocator racingLocator;
    racingLocator.registerService(service);
    std::barrier startRace{2};
    std::atomic_uint successfulRaceReads{};
    std::atomic_uint rejectedRaceReads{};
    std::jthread racingReader([&] {
        startRace.arrive_and_wait();
        for (unsigned int iteration = 0; iteration < 10'000; ++iteration) {
            try {
                static_cast<void>(racingLocator.getService<Service>());
                ++successfulRaceReads;
            } catch (const std::runtime_error&) {
                ++rejectedRaceReads;
            }
        }
    });
    startRace.arrive_and_wait();
    racingLocator.unregisterService<Service>();
    racingReader.join();

    return parallelReadsSucceeded
        && lookupRejected
        && consumer == service
        && registered == 1
        && duplicates == 7
        && successfulRaceReads + rejectedRaceReads == 10'000
        ? 0
        : 1;
}
