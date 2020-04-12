#include "fast_ber/util/BerContainer.hpp"
#include "fast_ber/util/FixedIdBerContainer.hpp"
#include "fast_ber/util/SmallFixedIdBerContainer.hpp"

#include <catch2/catch.hpp>

#include <array>

template <typename Container>
void empty_construction()
{
    Container  container;
    const auto expected = std::array<uint8_t, 2>({0x00, 0x00});

    CHECK(container.view().is_valid());
    CHECK(container.view().construction() == fast_ber::Construction::primitive);
    CHECK(container.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
    CHECK(container.class_() == fast_ber::Class::universal);
    CHECK(container.tag() == 0);
    CHECK(container.content_length() == 0);
    CHECK(container.ber() == absl::MakeSpan(expected.data(), expected.size()));
}

template <typename Container>
void resize_larger(size_t new_size = 99999)
{
    const auto test_data = std::array<uint8_t, 3>({'a', 'b', 'c'});
    Container  container(test_data, fast_ber::ConstructionMethod::construct_with_provided_content);

    CHECK(container.view().is_valid());
    CHECK(container.tag() == 0);
    CHECK(container.content_length() == test_data.size());
    CHECK(container.content() == test_data);

    container.resize_content(new_size);

    CHECK(container.view().is_valid());
    CHECK(container.tag() == 0);
    CHECK(container.content_length() == new_size);
    CHECK(absl::MakeSpan(container.content_data(), test_data.size()) == test_data);
    CHECK(container.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
}

template <typename Container>
void resize_smaller()
{
    const auto             test_data = std::array<uint8_t, 9>({'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'});
    fast_ber::BerContainer container(test_data, fast_ber::ConstructionMethod::construct_with_provided_content);

    size_t new_size = 5;

    CHECK(container.is_valid());
    CHECK(container.tag() == 0);
    CHECK(container.content_length() == test_data.size());
    CHECK(container.content() == test_data);

    container.resize_content(new_size);

    CHECK(container.is_valid());
    CHECK(container.tag() == 0);
    CHECK(container.content_length() == new_size);
    CHECK(container.content() == absl::MakeSpan(test_data.data(), new_size));
    CHECK(container.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
}

template <typename Container>
void resize_same_size()
{
    const auto             test_data = std::array<uint8_t, 9>({'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'});
    fast_ber::BerContainer container(test_data, fast_ber::ConstructionMethod::construct_with_provided_content);

    size_t new_size = test_data.size();

    CHECK(container.is_valid());
    CHECK(container.tag() == 0);
    CHECK(container.content_length() == test_data.size());
    CHECK(container.content() == test_data);

    container.resize_content(new_size);

    CHECK(container.is_valid());
    CHECK(container.tag() == 0);
    CHECK(container.content_length() == new_size);
    CHECK(container.content() == test_data);
    CHECK(container.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
}

template <typename Container>
void assign()
{
    std::array<uint8_t, 11> test_pdu = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd'};

    Container container1(test_pdu, fast_ber::ConstructionMethod::construct_with_provided_content);
    Container container2 = container1;
    Container container3 = std::move(Container{container1});
    Container container4;
    Container container5;
    Container container6;
    Container container7;

    fast_ber::DecodeResult res = container4.decode(fast_ber::BerView(container1.ber()));
    container5.assign_content(container1.content());
    container6 = container1;
    container7 = std::move(Container{container1});

    CHECK(res.success);
    CHECK(container1.content() == test_pdu);
    CHECK(container2.content() == test_pdu);
    CHECK(container3.content() == test_pdu);
    CHECK(container4.content() == test_pdu);
    CHECK(container5.content() == test_pdu);
    CHECK(container6.content() == test_pdu);
    CHECK(container7.content() == test_pdu);

    CHECK(container1.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
    CHECK(container2.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
    CHECK(container3.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
    CHECK(container4.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
    CHECK(container5.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
    CHECK(container6.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
    CHECK(container7.view().identifier() == fast_ber::RuntimeId{fast_ber::Class::universal, 0});
}

template <typename Container>
void encode_decode()
{
    std::array<uint8_t, 100> buffer = {};

    Container container;
    container.assign_content(std::vector<uint8_t>(50, 0));

    fast_ber::EncodeResult encode_res = container.encode(absl::Span<uint8_t>(buffer));
    fast_ber::DecodeResult decode_res = container.decode(fast_ber::BerView(buffer));

    REQUIRE(encode_res.success);
    REQUIRE(decode_res.success);
    REQUIRE(encode_res.length == container.ber().length());
}

using SingleTestId = fast_ber::Id<fast_ber::Class::universal, 0>;
using DoubleTestId = fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::universal, 0>,
                                        fast_ber::ExplicitId<fast_ber::UniversalTag::sequence>>;
using LongTestId   = fast_ber::DoubleId<fast_ber::Id<fast_ber::Class::universal, 0>,
                                      fast_ber::Id<fast_ber::Class::context_specific, 9999999999999>>;

TEST_CASE("BerContainer: Empty construction")
{
    empty_construction<fast_ber::BerContainer>();
    empty_construction<fast_ber::FixedIdBerContainer<SingleTestId>>();
    empty_construction<fast_ber::SmallFixedIdBerContainer<SingleTestId, 0>>();
}

TEST_CASE("BerContainer: Resize larger")
{
    resize_larger<fast_ber::BerContainer>();
    resize_larger<fast_ber::FixedIdBerContainer<SingleTestId>>();
    resize_larger<fast_ber::FixedIdBerContainer<DoubleTestId>>();
    resize_larger<fast_ber::FixedIdBerContainer<LongTestId>>();
    resize_larger<fast_ber::SmallFixedIdBerContainer<SingleTestId, 75>>(75);
    resize_larger<fast_ber::SmallFixedIdBerContainer<DoubleTestId, 75>>(73);
    resize_larger<fast_ber::SmallFixedIdBerContainer<LongTestId, 75>>(66);
}

TEST_CASE("BerContainer: Resize smaller")
{
    resize_smaller<fast_ber::BerContainer>();
    resize_smaller<fast_ber::FixedIdBerContainer<SingleTestId>>();
    resize_smaller<fast_ber::FixedIdBerContainer<DoubleTestId>>();
    resize_smaller<fast_ber::FixedIdBerContainer<LongTestId>>();
    resize_smaller<fast_ber::SmallFixedIdBerContainer<SingleTestId, 9>>();
    resize_smaller<fast_ber::SmallFixedIdBerContainer<DoubleTestId, 9>>();
    resize_smaller<fast_ber::SmallFixedIdBerContainer<LongTestId, 9>>();
}

TEST_CASE("BerContainer: Resize same size")
{
    resize_same_size<fast_ber::BerContainer>();
    resize_same_size<fast_ber::FixedIdBerContainer<SingleTestId>>();
    resize_same_size<fast_ber::FixedIdBerContainer<DoubleTestId>>();
    resize_same_size<fast_ber::FixedIdBerContainer<LongTestId>>();
    resize_same_size<fast_ber::SmallFixedIdBerContainer<SingleTestId, 11>>();
    resize_same_size<fast_ber::SmallFixedIdBerContainer<DoubleTestId, 11>>();
    resize_same_size<fast_ber::SmallFixedIdBerContainer<LongTestId, 11>>();
}

TEST_CASE("BerContainer: Assign")
{
    assign<fast_ber::BerContainer>();
    assign<fast_ber::FixedIdBerContainer<SingleTestId>>();
    assign<fast_ber::FixedIdBerContainer<DoubleTestId>>();
    assign<fast_ber::FixedIdBerContainer<LongTestId>>();
    assign<fast_ber::SmallFixedIdBerContainer<SingleTestId, 11>>();
    assign<fast_ber::SmallFixedIdBerContainer<DoubleTestId, 11>>();
    assign<fast_ber::SmallFixedIdBerContainer<LongTestId, 11>>();
}

TEST_CASE("BerContainer: Encode and Decode")
{
    encode_decode<fast_ber::BerContainer>();
    encode_decode<fast_ber::FixedIdBerContainer<SingleTestId>>();
    encode_decode<fast_ber::FixedIdBerContainer<DoubleTestId>>();
    encode_decode<fast_ber::FixedIdBerContainer<LongTestId>>();
    encode_decode<fast_ber::SmallFixedIdBerContainer<SingleTestId, 50>>();
    encode_decode<fast_ber::SmallFixedIdBerContainer<DoubleTestId, 50>>();
    encode_decode<fast_ber::SmallFixedIdBerContainer<LongTestId, 50>>();
}

TEST_CASE("BerContainer: FixedIdBerContainer IDs")
{
    fast_ber::FixedIdBerContainer<SingleTestId> single_id;
    fast_ber::FixedIdBerContainer<DoubleTestId> double_id;
    fast_ber::FixedIdBerContainer<LongTestId>   long_id;

    CHECK(SingleTestId{} == fast_ber::BerView(single_id.ber()).identifier());

    CHECK(DoubleTestId{}.outer_id() == double_id.view().identifier());
    CHECK(DoubleTestId{}.inner_id() == double_id.view().begin()->identifier());

    CHECK(LongTestId{}.outer_id() == long_id.view().identifier());
    CHECK(LongTestId{}.inner_id() == long_id.view().begin()->identifier());
}

TEST_CASE("BerContainer: SmallFixedIdBerContainer IDs")
{
    fast_ber::SmallFixedIdBerContainer<SingleTestId, 5> single_id;
    fast_ber::SmallFixedIdBerContainer<DoubleTestId, 5> double_id;
    fast_ber::SmallFixedIdBerContainer<LongTestId, 5>   long_id;

    CHECK(SingleTestId{} == fast_ber::BerView(single_id.ber()).identifier());

    CHECK(DoubleTestId{}.outer_id() == double_id.view().identifier());
    CHECK(DoubleTestId{}.inner_id() == double_id.view().begin()->identifier());

    CHECK(LongTestId{}.outer_id() == long_id.view().identifier());
    CHECK(LongTestId{}.inner_id() == long_id.view().begin()->identifier());
}

TEST_CASE("BerContainer: FixedIdBerContainer Different ID Assign")
{
    fast_ber::FixedIdBerContainer<SingleTestId> single_id;
    fast_ber::FixedIdBerContainer<DoubleTestId> double_id;
    fast_ber::FixedIdBerContainer<LongTestId>   long_id;

    single_id.assign_content(std::vector<uint8_t>(100, 'a'));
    double_id.assign_content(std::vector<uint8_t>(50, 'f'));
    long_id.assign_content(std::vector<uint8_t>(10, 'z'));

    CHECK(SingleTestId{} == fast_ber::BerView(single_id.ber()).identifier());
    CHECK(DoubleTestId{}.outer_id() == double_id.view().identifier());
    CHECK(DoubleTestId{}.inner_id() == double_id.view().begin()->identifier());
    CHECK(LongTestId{}.outer_id() == long_id.view().identifier());
    CHECK(LongTestId{}.inner_id() == long_id.view().begin()->identifier());

    double_id = long_id = single_id;

    CHECK(SingleTestId{} == fast_ber::BerView(single_id.ber()).identifier());
    CHECK(DoubleTestId{}.outer_id() == double_id.view().identifier());
    CHECK(DoubleTestId{}.inner_id() == double_id.view().begin()->identifier());
    CHECK(LongTestId{}.outer_id() == long_id.view().identifier());
    CHECK(LongTestId{}.inner_id() == long_id.view().begin()->identifier());

    CHECK(double_id.content() == single_id.content());
    CHECK(long_id.content() == single_id.content());
}

TEST_CASE("BerContainer: SmallFixedIdBerContainer Different ID Assign")
{
    fast_ber::SmallFixedIdBerContainer<SingleTestId, 128> single_id;
    fast_ber::SmallFixedIdBerContainer<DoubleTestId, 126> double_id;
    fast_ber::SmallFixedIdBerContainer<LongTestId, 119>   long_id;

    single_id.assign_content(std::vector<uint8_t>(100, 'a'));
    double_id.assign_content(std::vector<uint8_t>(50, 'f'));
    long_id.assign_content(std::vector<uint8_t>(10, 'z'));

    CHECK(SingleTestId{} == fast_ber::BerView(single_id.ber()).identifier());
    CHECK(DoubleTestId{}.outer_id() == double_id.view().identifier());
    CHECK(DoubleTestId{}.inner_id() == double_id.view().begin()->identifier());
    CHECK(LongTestId{}.outer_id() == long_id.view().identifier());
    CHECK(LongTestId{}.inner_id() == long_id.view().begin()->identifier());

    double_id = long_id = single_id;

    CHECK(SingleTestId{} == fast_ber::BerView(single_id.ber()).identifier());
    CHECK(DoubleTestId{}.outer_id() == double_id.view().identifier());
    CHECK(DoubleTestId{}.inner_id() == double_id.view().begin()->identifier());
    CHECK(LongTestId{}.outer_id() == long_id.view().identifier());
    CHECK(LongTestId{}.inner_id() == long_id.view().begin()->identifier());

    CHECK(double_id.content() == single_id.content());
    CHECK(long_id.content() == single_id.content());
}
