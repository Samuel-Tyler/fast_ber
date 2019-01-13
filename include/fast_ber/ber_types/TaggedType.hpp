#pragma once

namespace fast_ber
{

template <typename Type, typename TagType>
class TaggedType
{
  public:
    // Implicitly convert to underlying (untagged) type
    operator Type&() const noexcept { return type; }

  private:
    Type    type;
    TagType tag;
};

} // namespace fast_ber
