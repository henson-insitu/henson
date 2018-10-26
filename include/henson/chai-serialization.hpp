#ifndef HENSON_CHAI_SERIALIZATION_HPP
#define HENSON_CHAI_SERIALIZATION_HPP

#include <chaiscript/dispatchkit/boxed_cast.hpp>
#include <chaiscript/dispatchkit/boxed_value.hpp>

#include <henson/serialization.hpp>
#include <henson/data.hpp>

namespace henson
{
    namespace detail
    {
        template<class T>
        bool is_boxed(const chaiscript::Boxed_Value& bv)
        {
            try
            {
                chaiscript::boxed_cast<T>(bv);
                return true;
            } catch (...)
            {
                return false;
            }
        }
    }

    template<>
    struct Serialization<chaiscript::Boxed_Value>
    {
        struct tags
        {
            enum
            {
                int_, float_, double_, size_t_, string_, array_, vector_, undef_
            };
        };

        static void         save(BinaryBuffer& bb, const chaiscript::Boxed_Value& bv)
        {
            if (detail::is_boxed<int>(bv))
            {
                henson::save(bb, tags::int_);
                henson::save(bb, chaiscript::boxed_cast<int>(bv));
            } else if (detail::is_boxed<float>(bv))
            {
                henson::save(bb, tags::float_);
                henson::save(bb, chaiscript::boxed_cast<float>(bv));
            } else if (detail::is_boxed<double>(bv))
            {
                henson::save(bb, tags::double_);
                henson::save(bb, chaiscript::boxed_cast<double>(bv));
            } else if (detail::is_boxed<size_t>(bv))
            {
                henson::save(bb, tags::size_t_);
                henson::save(bb, chaiscript::boxed_cast<size_t>(bv));
            } else if (detail::is_boxed<std::string>(bv))
            {
                henson::save(bb, tags::string_);
                henson::save(bb, chaiscript::boxed_cast<std::string>(bv));
            } else if (detail::is_boxed<std::vector<chaiscript::Boxed_Value>>(bv))
            {
                henson::save(bb, tags::vector_);
                henson::save(bb, chaiscript::boxed_cast<std::vector<chaiscript::Boxed_Value>>(bv));
            } else if (detail::is_boxed<Array>(bv))
            {
                henson::save(bb, tags::array_);
                henson::save(bb, chaiscript::boxed_cast<Array>(bv));
            } else if (bv.get_type_info().is_undef())
                henson::save(bb, tags::undef_);
            else
                throw std::runtime_error("Cannot serialize a boxed value");
        }

        static void         load(BinaryBuffer& bb, chaiscript::Boxed_Value& bv)
        {
            int tag;
            henson::load(bb, tag);
            if (tag == tags::int_)
            {
                int i;
                henson::load(bb, i);
                bv = chaiscript::Boxed_Value(i);
            } else if (tag == tags::float_)
            {
                float f;
                henson::load(bb, f);
                bv = chaiscript::Boxed_Value(f);
            } else if (tag == tags::double_)
            {
                double d;
                henson::load(bb, d);
                bv = chaiscript::Boxed_Value(d);
            } else if (tag == tags::size_t_)
            {
                size_t s;
                henson::load(bb, s);
                bv = chaiscript::Boxed_Value(s);
            } else if (tag == tags::string_)
            {
                std::string s;
                henson::load(bb, s);
                bv = chaiscript::Boxed_Value(s);
            } else if (tag == tags::vector_)
            {
                std::vector<chaiscript::Boxed_Value> v;
                henson::load(bb, v);
                bv = chaiscript::Boxed_Value(v);
            } else if (tag == tags::array_)
            {
                Array a;
                henson::load(bb, a);
                bv = chaiscript::Boxed_Value(a);
            } else if (tag == tags::undef_)
                bv = chaiscript::Boxed_Value();
            else
                throw std::runtime_error("Cannot deserialize a boxed value");
        }
    };
}

#endif
