#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace Uncarved::ContentSpace
{
    enum class FileErrorType
    {
        None,
        InvalidPath,
        NotFound
    };

    struct FileError final
    {
        std::string   errorMessage_{};
        FileErrorType errorType_{FileErrorType::None};
    };

    enum class ParseErrorType
    {
        None,
        ParseFailed,
        InvalidStructure
    };

    struct ParseError final
    {
        std::string    errorMessage_{};
        ParseErrorType errorType_{ParseErrorType::None};
    };

    enum class ValidationErrorType
    {
        None,
        InvalidData
    };

    struct ValidationError final
    {
        std::string         errorMessage_{};
        ValidationErrorType errorType_{ValidationErrorType::None};
    };

    enum class ResourceErrorType
    {
        None,
        LoadFailed
    };

    struct ResourceError final
    {
        std::string       errorMessage_{};
        ResourceErrorType errorType_{ResourceErrorType::None};
    };

    using ContentError = std::variant<FileError, ParseError, ValidationError, ResourceError>;

    struct ContentResult final
    {
        std::optional<ContentError> contentError_;

        bool isSucceeded() const noexcept
        {
            return !contentError_.has_value();
        }

        std::string_view getErrorMessage() const noexcept
        {
            if (!contentError_)
            {
                return {};
            }

            return std::visit(
                [](const auto& contentError) -> std::string_view { return contentError.errorMessage_; },
                *contentError_
            );
        }
    };
} // namespace Uncarved::ContentSpace
