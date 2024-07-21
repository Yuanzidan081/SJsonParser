#include "Json.h"
#include "JsonValue.h"
#include "JsonException.h"
namespace SJson
{
    Json::Json() noexcept : m_Value(new JsonValue) {}
    Json::~Json() noexcept {}
    Json::Json(const Json &rhs) noexcept
    {
        m_Value.reset(new JsonValue(*(rhs.m_Value)));
    }
    Json &Json::operator=(const Json &rhs) noexcept
    {
        if (this == &rhs)
            return *this;
        m_Value.reset(new JsonValue(*(rhs.m_Value)));
        return *this;
    }
    Json::Json(Json &&rhs) noexcept
    {
        m_Value.reset(rhs.m_Value.release());
    }

    Json &Json::operator=(Json &&rhs) noexcept
    {
        m_Value.reset(rhs.m_Value.release());
        return *this;
    }
    void Json::swap(Json &rhs) noexcept
    {
        using std::swap;
        swap(m_Value, rhs.m_Value);
    }

    void Json::Parse(const std::string &content, std::string &status) noexcept
    {
        try
        {
            Parse(content);
            status = "parse ok";
        }
        catch (const JsonException &msg)
        {
            status = msg.what();
        }
        catch (...)
        {
        }
    }

    void Json::Parse(const std::string &content)
    {
        m_Value->Parse(content);
    }

    bool operator==(const Json &lhs, const Json &rhs) noexcept
    {
        return *(lhs.m_Value) == *(rhs.m_Value);
    }

    bool operator!=(const Json &lhs, const Json &rhs) noexcept
    {
        return *(lhs.m_Value) != *(rhs.m_Value);
    }

    void swap(Json &lhs, Json &rhs) noexcept
    {
        lhs.swap(rhs);
    }

    int Json::GetType() const noexcept
    {
        if (m_Value == nullptr)
            return JsonType::Null;
        return m_Value->GetType();
    }
    void Json::SetBoolean(bool b) noexcept
    {
        if (b)
            m_Value->SetType(JsonType::True);
        else
            m_Value->SetType(JsonType::False);
    }
}