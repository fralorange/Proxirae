#pragma once

#include <chrono>
#include <utility>
#include <type_traits>

#include "features/proxification/stream/DiagnosticsCallback.h"
#include "features/proxification/stream/TestStage.h"
#include "platform/environment/sock_types.h"

namespace Proxirae {
    class TestableProxyBase {
    protected:
        DiagnosticsCallback m_callback;

        explicit TestableProxyBase(DiagnosticsCallback callback)
            : m_callback(std::move(callback)) {
        }

        virtual ~TestableProxyBase() = default;

        void Report(TestStage stage, std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end, bool success) const
        {
            if (m_callback) {
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
                m_callback(stage, ms, success);
            }
        }

        template <typename Func>
        auto MeasureStage(TestStage stage, Func&& func) const
        {
            auto start = std::chrono::steady_clock::now();
            auto result = func();
            auto end = std::chrono::steady_clock::now();

            bool success = false;
            if constexpr (std::is_same_v<std::decay_t<decltype(result)>, NativeSocket>) {
                success = (result != InvalidNativeSocket);
            }
            else {
                success = static_cast<bool>(result);
            }

            Report(stage, start, end, success);
            return result;
        }
    };
}