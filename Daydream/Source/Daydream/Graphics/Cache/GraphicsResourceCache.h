#pragma once

namespace Daydream
{
    class IGraphicsResourceCache
    {
    public:
        virtual ~IGraphicsResourceCache() = default;
        virtual void Clear() = 0;
    };

    template<typename Key, typename Value, typename Hash>
    class GraphicsResourceCache : public IGraphicsResourceCache
    {
    public:
        virtual ~GraphicsResourceCache() override = default;

        Value* Request(const Key& _key)
        {
            auto it = cache.find(_key);
            if (it != cache.end())
            {
                return it->second.get();
            }
            Shared<Value> newValue = CreateResource(_key);
            cache.insert({ _key, newValue });

            return newValue.get();
        }

        void Clear() override { cache.clear(); }
    protected:
        virtual Shared<Value> CreateResource(const Key& _key) = 0;

    private:
        HashMap<Key, Shared<Value>, Hash> cache;
    };
}
