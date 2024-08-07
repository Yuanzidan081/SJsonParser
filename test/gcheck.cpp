#include <gtest/gtest.h>
#include "../src/Json.h"
#include <string>

static std::string status;

#define test_literal(expect, content)   \
    do                                  \
    {                                   \
        SJson::Json j;                  \
        j.SetBoolean(false);            \
        j.Parse(content, status);       \
        EXPECT_EQ("parse ok", status);  \
        EXPECT_EQ(expect, j.GetType()); \
    } while (0)

TEST(TestLiteral, NullTrueFalse)
{
    using namespace SJson;
    test_literal(JsonType::Null, "null");
    test_literal(JsonType::True, "true");
    test_literal(JsonType::False, "false");
}

#define test_number(expect, content)              \
    do                                            \
    {                                             \
        SJson::Json j;                            \
        j.Parse(content, status);                 \
        EXPECT_EQ("parse ok", status);            \
        EXPECT_EQ(JsonType::Number, j.GetType()); \
        EXPECT_EQ(expect, j.GetNumber());         \
    } while (0)

TEST(TestNumber, Number)
{
    using namespace SJson;
    test_number(0.0, "0");
    test_number(0.0, "-0");
    test_number(0.0, "-0.0");
    test_number(1.0, "1");
    test_number(-1.0, "-1");
    test_number(1.5, "1.5");
    test_number(-1.5, "-1.5");
    test_number(3.1416, "3.1416");
    test_number(1E10, "1E10");
    test_number(1e10, "1e10");
    test_number(1E+10, "1E+10");
    test_number(1E-10, "1E-10");
    test_number(-1E10, "-1E10");
    test_number(-1e10, "-1e10");
    test_number(-1E+10, "-1E+10");
    test_number(-1E-10, "-1E-10");
    test_number(1.234E+10, "1.234E+10");
    test_number(1.234E-10, "1.234E-10");
    test_number(0.0, "1e-10000"); /* must underflow */
}

#define test_string(expect, content)                 \
    do                                               \
    {                                                \
        SJson::Json j;                               \
        j.Parse(content, status);                    \
        EXPECT_EQ("parse ok", status);               \
        EXPECT_EQ(JsonType::String, j.GetType());    \
        EXPECT_STREQ(expect, j.GetString().c_str()); \
    } while (0)

TEST(TestString, String)
{
    using namespace SJson;

    test_string("", "\"\"");
    test_string("Hello", "\"Hello\"");

    test_string("Hello\nWorld", "\"Hello\\nWorld\"");
    test_string("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");

    test_string("Hello\0World", "\"Hello\\u0000World\"");
    test_string("\x24", "\"\\u0024\"");                    /* Dollar sign U+0024 */
    test_string("\xC2\xA2", "\"\\u00A2\"");                /* Cents sign U+00A2 */
    test_string("\xE2\x82\xAC", "\"\\u20AC\"");            /* Euro sign U+20AC */
    test_string("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\""); /* G clef sign U+1D11E */
    test_string("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\""); /* G clef sign U+1D11E */
}

TEST(TestArray, Array)
{
    using namespace SJson;
    SJson::Json j;

    j.Parse("[ ]", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(JsonType::Array, j.GetType());
    EXPECT_EQ(0, j.GetArraySize());

    j.Parse("[ null , false , true , 123 , \"abc\" ]", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(JsonType::Array, j.GetType());
    EXPECT_EQ(5, j.GetArraySize());
    EXPECT_EQ(JsonType::Null, j.GetArrayElement(0).GetType());
    EXPECT_EQ(JsonType::False, j.GetArrayElement(1).GetType());
    EXPECT_EQ(JsonType::True, j.GetArrayElement(2).GetType());
    EXPECT_EQ(JsonType::Number, j.GetArrayElement(3).GetType());
    EXPECT_EQ(JsonType::String, j.GetArrayElement(4).GetType());
    EXPECT_EQ(123.0, j.GetArrayElement(3).GetNumber());
    EXPECT_STREQ("abc", j.GetArrayElement(4).GetString().c_str());

    j.Parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(JsonType::Array, j.GetType());
    EXPECT_EQ(4, j.GetArraySize());
    for (int i = 0; i < 4; i++)
    {
        SJson::Json a = j.GetArrayElement(i);
        EXPECT_EQ(JsonType::Array, a.GetType());
        for (int k = 0; k < i; k++)
        {
            SJson::Json b = a.GetArrayElement(k);
            EXPECT_EQ(JsonType::Number, b.GetType());
            EXPECT_EQ((double)k, b.GetNumber());
        }
    }
}

TEST(TestObject, Object)
{
    using namespace SJson;
    SJson::Json v;

    v.Parse(" { } ", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(JsonType::Object, v.GetType());
    EXPECT_EQ(0, v.GetObjectSize());

    v.Parse(" { "
            "\"n\" : null , "
            "\"f\" : false , "
            "\"t\" : true , "
            "\"i\" : 123 , "
            "\"s\" : \"abc\", "
            "\"a\" : [ 1, 2, 3 ],"
            "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
            " } ",
            status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(JsonType::Object, v.GetType());
    EXPECT_EQ(7, v.GetObjectSize());
    EXPECT_EQ("n", v.GetObjectKey(0));
    EXPECT_EQ(JsonType::Null, v.GetObjectValue(0).GetType());
    EXPECT_EQ("f", v.GetObjectKey(1));
    EXPECT_EQ(JsonType::False, v.GetObjectValue(1).GetType());
    EXPECT_EQ("t", v.GetObjectKey(2));
    EXPECT_EQ(JsonType::True, v.GetObjectValue(2).GetType());
    EXPECT_EQ("i", v.GetObjectKey(3));
    EXPECT_EQ(JsonType::Number, v.GetObjectValue(3).GetType());
    EXPECT_EQ(123.0, v.GetObjectValue(3).GetNumber());
    EXPECT_EQ("s", v.GetObjectKey(4));
    EXPECT_EQ(JsonType::String, v.GetObjectValue(4).GetType());
    EXPECT_EQ("abc", v.GetObjectValue(4).GetString());
    EXPECT_EQ("a", v.GetObjectKey(5));
    EXPECT_EQ(JsonType::Array, v.GetObjectValue(5).GetType());
    EXPECT_EQ(3, v.GetObjectValue(5).GetArraySize());
    for (int i = 0; i < 3; ++i)
    {
        SJson::Json e = v.GetObjectValue(5).GetArrayElement(i);
        EXPECT_EQ(JsonType::Number, e.GetType());
        EXPECT_EQ(i + 1.0, e.GetNumber());
    }
    EXPECT_EQ("o", v.GetObjectKey(6));
    {
        SJson::Json o = v.GetObjectValue(6);
        EXPECT_EQ(JsonType::Object, o.GetType());
        for (int i = 0; i < 3; i++)
        {
            SJson::Json ov = o.GetObjectValue(i);
            EXPECT_EQ('1' + i, (o.GetObjectKey(i))[0]); // (o.get_object_key(i))[0] 表示获得string的第一个字符
            EXPECT_EQ(1, o.GetObjectKeyLength(i));
            EXPECT_EQ(JsonType::Number, ov.GetType());
            EXPECT_EQ(i + 1.0, ov.GetNumber());
        }
    }
}

#define test_error(error, content)                \
    do                                            \
    {                                             \
        SJson::Json v;                            \
        v.Parse(content, status);                 \
        EXPECT_EQ(error, status);                 \
        EXPECT_EQ((JsonType::Null), v.GetType()); \
    } while (0)

// 测试解析期望值
TEST(TestParseExpectValue, ParseExpectValue)
{
    using namespace SJson;
    test_error("parse expect value", "");
    test_error("parse expect value", " ");
}

// 测试解析无效值
TEST(TestParseInvalidValue, ParseInvalidValue)
{
    using namespace SJson;
    test_error("parse invalid value", "nul");
    test_error("parse invalid value", "!?");

    test_error("parse invalid value", "+0");
    test_error("parse invalid value", "+1");
    test_error("parse invalid value", ".123"); // 在小数点之前必须有一位数字
    test_error("parse invalid value", "1.");   // 在小数点之后必须有一位数字
    test_error("parse invalid value", "INF");
    test_error("parse invalid value", "NAN");
    test_error("parse invalid value", "nan");

#if 1
    test_error("parse invalid value", "[1,]");
    test_error("parse invalid value", "[\"a\", nul]");
#endif
}

// 测试解析非单数根
TEST(TestParseRootNotSingular, ParseRootNotSingular)
{
    using namespace SJson;
    test_error("parse root not singular", "null x");
    test_error("parse root not singular", "truead");
    test_error("parse root not singular", "\"dsad\"d");

    test_error("parse root not singular", "0123");
    test_error("parse root not singular", "0x0");
    test_error("parse root not singular", "0x123");
}

// 测试解析数字太大
TEST(TestParseNumberTooBig, ParseNumberTooBig)
{
    using namespace SJson;
    test_error("parse number too big", "1e309");
    test_error("parse number too big", "-1e309");
}

// 测试解析缺失引号
TEST(TestParseMissingQuotationMark, ParseMissingQuotationMark)
{
    using namespace SJson;
    test_error("parse miss quotation mark", "\"");
    test_error("parse miss quotation mark", "\"abc");
}

// 测试解析无效字符串转义
TEST(TestParseInvalidStringEscape, ParseInvalidStringEscape)
{
    using namespace SJson;
#if 1
    test_error("parse invalid string escape", "\"\\v\"");
    test_error("parse invalid string escape", "\"\\'\"");
    test_error("parse invalid string escape", "\"\\0\"");
    test_error("parse invalid string escape", "\"\\x12\"");
#endif
}

// 测试解析无效字符串字符
TEST(TestParseInvalidStringChar, ParseInvalidStringChar)
{
    using namespace SJson;
#if 1
    test_error("parse invalid string char", "\"\x01\"");
    test_error("parse invalid string char", "\"\x1F\"");
#endif
}

// 测试解析无效的unicode十六进制
TEST(TestParseInvalidUnicodeHex, ParseInvalidUnicodeHex)
{
    using namespace SJson;
    test_error("parse invalid unicode hex", "\"\\u\"");
    test_error("parse invalid unicode hex", "\"\\u0\"");
    test_error("parse invalid unicode hex", "\"\\u01\"");
    test_error("parse invalid unicode hex", "\"\\u012\"");
    test_error("parse invalid unicode hex", "\"\\u/000\"");
    test_error("parse invalid unicode hex", "\"\\uG000\"");
    test_error("parse invalid unicode hex", "\"\\u0/00\"");
    test_error("parse invalid unicode hex", "\"\\u0G00\"");
    test_error("parse invalid unicode hex", "\"\\u0/00\"");
    test_error("parse invalid unicode hex", "\"\\u00G0\"");
    test_error("parse invalid unicode hex", "\"\\u000/\"");
    test_error("parse invalid unicode hex", "\"\\u000G\"");
    test_error("parse invalid unicode hex", "\"\\u 123\"");
}

// 测试解析无效的unicode代理
TEST(TestParseInvalidUnicodeSurrogate, ParseInvalidUnicodeSurrogate)
{
    using namespace SJson;
    test_error("parse invalid unicode surrogate", "\"\\uD800\"");
    test_error("parse invalid unicode surrogate", "\"\\uDBFF\"");
    test_error("parse invalid unicode surrogate", "\"\\uD800\\\\\"");
    test_error("parse invalid unicode surrogate", "\"\\uD800\\uDBFF\"");
    test_error("parse invalid unicode surrogate", "\"\\uD800\\uE000\"");
}

// 测试解析缺失逗号或方括号
TEST(TestParseMissCommaOrSquareBracket, ParseMissCommaOrSquareBracket)
{
    using namespace SJson;
#if 1
    test_error("parse miss comma or square bracket", "[1");
    test_error("parse miss comma or square bracket", "[1}");
    test_error("parse miss comma or square bracket", "[1 2");
    test_error("parse miss comma or square bracket", "[[]");
#endif
}

// 测试解析缺失key值
TEST(TestParseMissKey, ParseMissKey)
{
    using namespace SJson;
    test_error("parse miss key", "{:1,");
    test_error("parse miss key", "{1:1,");
    test_error("parse miss key", "{true:1,");
    test_error("parse miss key", "{false:1,");
    test_error("parse miss key", "{null:1,");
    test_error("parse miss key", "{[]:1,");
    test_error("parse miss key", "{{}:1,");
    test_error("parse miss key", "{\"a\":1,");
}

// 测试解析漏掉逗号或大括号
TEST(TestParseMissCommaOrCurlyBracket, ParseMissCommaOrCurlyBracket)
{
    using namespace SJson;
    test_error("parse miss comma or curly bracket", "{\"a\":1");
    test_error("parse miss comma or curly bracket", "{\"a\":1]");
    test_error("parse miss comma or curly bracket", "{\"a\":1 \"b\"");
    test_error("parse miss comma or curly bracket", "{\"a\":{}");
}

// 往返测试：把一个 JSON 解析，然后再生成另一 JSON，逐字符比较两个 JSON 是否一模一样。
// 先将 content 进行解析，然后判断是否解析成功；再然后将 v 生成一个 json 值存储在 status 中，最后比较 content 和 status 是否一样，这样就完成往返测试了
#define test_roundtrip(content)        \
    do                                 \
    {                                  \
        SJson::Json v;                 \
        v.Parse(content, status);      \
        EXPECT_EQ("parse ok", status); \
        v.Stringify(status);           \
        EXPECT_EQ(content, status);    \
    } while (0)

// 测试序列化数字
TEST(TestStringifyNumber, StringifyNumber)
{
    test_roundtrip("0");
    test_roundtrip("-0");
    test_roundtrip("1");
    test_roundtrip("-1");
    test_roundtrip("1.5");
    test_roundtrip("-1.5");
    test_roundtrip("3.25");
    test_roundtrip("1e+20");
    test_roundtrip("1.234e+20");
    test_roundtrip("1.234e-20");

    test_roundtrip("1.0000000000000002");      /* the smallest number > 1 */
    test_roundtrip("4.9406564584124654e-324"); /* minimum denormal */
    test_roundtrip("-4.9406564584124654e-324");
    test_roundtrip("2.2250738585072009e-308"); /* Max subnormal double */
    test_roundtrip("-2.2250738585072009e-308");
    test_roundtrip("2.2250738585072014e-308"); /* Min normal positive double */
    test_roundtrip("-2.2250738585072014e-308");
    test_roundtrip("1.7976931348623157e+308"); /* Max double */
    test_roundtrip("-1.7976931348623157e+308");
}

// 测试序列化字符串
TEST(TestStringifyString, StringifyString)
{
    test_roundtrip("\"\"");
    test_roundtrip("\"Hello\"");
    test_roundtrip("\"Hello\\nWorld\"");
    test_roundtrip("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    test_roundtrip("\"Hello\\u0000World\"");
}

// 测试序列化数组
TEST(TestStringifyArray, StringifyArray)
{
    test_roundtrip("[]");
    test_roundtrip("[null,false,true,123,\"abc\",[1,2,3]]");
}

// 测试序列化对象
TEST(TestStringifyObject, StringifyObject)
{
    test_roundtrip("{}");
    test_roundtrip("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}

// 测试序列化的 null、false、true
TEST(TestStringify, Stringify)
{
    test_roundtrip("null");
    test_roundtrip("true");
    test_roundtrip("false");
}

#define test_equal(json1, json2, equality)  \
    do                                      \
    {                                       \
        SJson::Json v1, v2;                 \
        v1.Parse(json1, status);            \
        EXPECT_EQ("parse ok", status);      \
        v2.Parse(json2, status);            \
        EXPECT_EQ("parse ok", status);      \
        EXPECT_EQ(equality, int(v1 == v2)); \
    } while (0)

// 测试是否相等
TEST(TestEqual, Equal)
{
    test_equal("true", "true", 1);
    test_equal("true", "false", 0);
    test_equal("false", "false", 1);
    test_equal("null", "null", 1);
    test_equal("null", "0", 0);
    test_equal("123", "123", 1);
    test_equal("123", "456", 0);
    test_equal("\"abc\"", "\"abc\"", 1);
    test_equal("\"abc\"", "\"abcd\"", 0);
    test_equal("[]", "[]", 1);
    test_equal("[]", "null", 0);
    test_equal("[1,2,3]", "[1,2,3]", 1);
    test_equal("[1,2,3]", "[1,2,3,4]", 0);
    test_equal("[[]]", "[[]]", 1);
    test_equal("{}", "{}", 1);
    test_equal("{}", "null", 0);
    test_equal("{}", "[]", 0);
    test_equal("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2}", 1);
    test_equal("{\"a\":1,\"b\":2}", "{\"b\":2,\"a\":1}", 1);
    test_equal("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":3}", 0);
    test_equal("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2,\"c\":3}", 0);
    test_equal("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":{}}}}", 1);
    test_equal("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":[]}}}", 0);
}

// 测试是否拷贝
TEST(TestCopy, Copy)
{
    SJson::Json v1, v2;
    v1.Parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
    v2 = v1;
    EXPECT_EQ(1, int(v2 == v1));
}

// 测试是否移动
TEST(TestMove, Move)
{
    using namespace SJson;
    SJson::Json v1, v2, v3;
    v1.Parse("{\"t\":true, \"f\":false, \"n\":null, \"d\":1.5, \"a\":[1,2,3]}");
    v2 = v1;
    v3 = std::move(v2);
    EXPECT_EQ(JsonType::Null, v2.GetType());
    EXPECT_EQ(1, int(v3 == v1));
}

// 测试是否交换
TEST(TestSwap, Swap)
{
    SJson::Json v1, v2;
    v1.SetString("Hello");
    v2.SetString("World!");
    SJson::swap(v1, v2);
    EXPECT_EQ("World!", v1.GetString());
    EXPECT_EQ("Hello", v2.GetString());
}

// 测试访问null
TEST(TestAccessNull, AccessNull)
{
    using namespace SJson;
    SJson::Json v;
    v.SetString("a");
    v.SetNull();
    EXPECT_EQ(JsonType::Null, v.GetType());
}

// 测试访问bool
TEST(TestAccessBoolean, AccessBoolean)
{
    using namespace SJson;
    SJson::Json v;
    v.SetString("a");
    v.SetBoolean(false);
    EXPECT_EQ(JsonType::False, v.GetType());

    v.SetString("a");
    v.SetBoolean(true);
    EXPECT_EQ(JsonType::True, v.GetType());
}

// 测试访问string
TEST(TestAccessString, AccessString)
{
    using namespace SJson;
    SJson::Json v;
    v.SetString("");
    EXPECT_EQ("", v.GetString());

    v.SetString("Hello");
    EXPECT_EQ("Hello", v.GetString());
}

// 测试访问array
TEST(TestAccessArray, AccessArray)
{
    using namespace SJson;
    SJson::Json a, e;

    for (size_t j = 0; j < 5; j += 5)
    {
        a.SetArray();
        EXPECT_EQ(0, a.GetArraySize());
        for (int i = 0; i < 10; ++i)
        {
            e.SetNumber(i);
            a.PushbackArrayElement(e);
        }

        EXPECT_EQ(10, a.GetArraySize());
        for (int i = 0; i < 10; ++i)
            EXPECT_EQ(static_cast<double>(i), a.GetArrayElement(i).GetNumber());
    }

    a.PopbackArrayElement();
    EXPECT_EQ(9, a.GetArraySize());
    for (int i = 0; i < 9; ++i)
        EXPECT_EQ(static_cast<double>(i), a.GetArrayElement(i).GetNumber());

    a.EraseArrayElement(4, 0);
    EXPECT_EQ(9, a.GetArraySize());
    for (int i = 0; i < 9; ++i)
        EXPECT_EQ(static_cast<double>(i), a.GetArrayElement(i).GetNumber());

    a.EraseArrayElement(8, 1);
    EXPECT_EQ(8, a.GetArraySize());
    for (int i = 0; i < 8; ++i)
        EXPECT_EQ(static_cast<double>(i), a.GetArrayElement(i).GetNumber());

    a.EraseArrayElement(0, 2);
    EXPECT_EQ(6, a.GetArraySize());
    for (int i = 0; i < 6; ++i)
        EXPECT_EQ(static_cast<double>(i + 2), a.GetArrayElement(i).GetNumber());

    for (int i = 0; i < 2; ++i)
    {
        e.SetNumber(i);
        a.InsertArrayElement(e, i);
    }

    EXPECT_EQ(8, a.GetArraySize());
    for (int i = 0; i < 8; ++i)
        EXPECT_EQ(static_cast<double>(i), a.GetArrayElement(i).GetNumber());

    e.SetString("Hello");
    a.PushbackArrayElement(e);

    a.ClearArray();
    EXPECT_EQ(0, a.GetArraySize());
}

// 测试访问object
TEST(TestAccessObject, AccessObject)
{
    using namespace SJson;
    SJson::Json o, v;
    for (int j = 0; j <= 5; j += 5)
    {
        o.SetObject();
        EXPECT_EQ(0, o.GetObjectSize());
        for (int i = 0; i < 10; ++i)
        {
            std::string key = "a";
            key[0] += i;
            v.SetNumber(i);
            o.SetObjectValue(key, v);
        }

        EXPECT_EQ(10, o.GetObjectSize());
        for (int i = 0; i < 10; ++i)
        {
            std::string key = "a";
            key[0] += i; // 'a'+1=b，这里就是改变字符
            auto index = o.FindObjectIndex(key);
            EXPECT_EQ(1, static_cast<int>(index >= 0));
            v = o.GetObjectValue(index);
            EXPECT_EQ(static_cast<double>(i), v.GetNumber());
        }
    }

    auto index = o.FindObjectIndex("j");
    EXPECT_EQ(1, static_cast<int>(index >= 0));
    o.RemoveObjectValue(index);
    index = o.FindObjectIndex("j");
    EXPECT_EQ(1, static_cast<int>(index < 0));
    EXPECT_EQ(9, o.GetObjectSize());

    index = o.FindObjectIndex("a");
    EXPECT_EQ(1, static_cast<int>(index >= 0));
    o.RemoveObjectValue(index);
    index = o.FindObjectIndex("a");
    EXPECT_EQ(1, static_cast<int>(index < 0));
    EXPECT_EQ(8, o.GetObjectSize());

    for (int i = 0; i < 8; i++)
    {
        std::string key = "a";
        key[0] += i + 1;
        EXPECT_EQ((double)i + 1, o.GetObjectValue(o.FindObjectIndex(key)).GetNumber());
    }

    v.SetString("Hello");
    o.SetObjectValue("World", v);

    index = o.FindObjectIndex("World");
    EXPECT_EQ(1, static_cast<int>(index >= 0));
    v = o.GetObjectValue(index);
    EXPECT_EQ("Hello", v.GetString());

    o.ClearObject();
    EXPECT_EQ(0, o.GetObjectSize());
}