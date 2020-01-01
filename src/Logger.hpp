/**********************************************************************************************
*
*   Ngine - A 2D game engine.
*
*   Copyright (C) 2020 NerdThings.
*
*   LICENSE: GNU LGPLv3
*   View: In Ngine.hpp
*
**********************************************************************************************/

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Config.hpp"

namespace NerdThings::Ngine {
    /**
     * Log writer class.
     *
     * One day this might even write to a file :O
     */
    class NEAPI Logger {
        template <typename ...Args>
        static std::string _getFormattedString(std::string format_, Args...args) {
            // Get the size
            auto size = snprintf(nullptr, 0, format_.c_str(), args...);

            // Get the string
            std::string output(size + 1, '\0');
#if defined(WIN32)
            sprintf_s(&output[0], size + 1, format_.c_str(), args...);
#else
            sprintf(&output[0], format_.c_str(), args...);
#endif

            return output;
        }

        /**
         * Write output to console.
         *
         * @todo Write to log file if enabled.
         */
        static void _write(std::string severity_, std::string moduleName_, std::string msg_) {
            std::cout << "[" << severity_ << "] (" << moduleName_ << "): " << msg_ << std::endl;
        }
    public:
        /**
         * Write a notice to the console.
         *
         * @tparam Args Parameter pack types.
         * @param moduleName_ The module.
         * @param format_ The string format.
         * @param args Anything extra.
         */
        template <typename ...Args>
        static void Notice(std::string moduleName_, std::string format_, Args...args) {
            _write("NOTICE", moduleName_, _getFormattedString(format_, args...));
        }

        /**
         * Write a warning to the console.
         *
         * @tparam Args Parameter pack types.
         * @param moduleName_ The module.
         * @param format_ The string format.
         * @param args Anything extra.
         */
        template <typename ...Args>
        static void Warn(std::string moduleName_, std::string format_, Args...args) {
            _write("WARN", moduleName_, _getFormattedString(format_, args...));
        }

        /**
         * Write an error to the console.
         *
         * @todo Allow throwing if error occurs
         * @tparam Args Parameter pack types.
         * @param moduleName_ The module.
         * @param format_ The string format.
         * @param args Anything extra.
         */
        template <typename ...Args>
        static void Error(std::string moduleName_, std::string format_, Args...args) {
            _write("ERROR", moduleName_, _getFormattedString(format_, args...));
        }

        /**
         * Write a fatal error to the console, then throw std::runtime_error with the error message.
         *
         * @tparam Args Parameter pack types.
         * @param moduleName_ The module.
         * @param format_ The string format.
         * @param args Anything extra.
         */
        template<typename ...Args>
        static void Fail(std::string moduleName_, std::string format_, Args... args) {
            auto msg = _getFormattedString(format_, args...);
            _write("FATAL", moduleName_, msg);
            throw std::runtime_error(msg); // TODO: Exception type tparam?
        }
    };
}

#endif //LOGGER_HPP