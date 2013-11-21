#pragma once

#include <vector>

namespace au
{
	// A simple id pool that allows for id reuse
    class IdPool
    {
    public:
        IdPool() : m_StartId(0), m_NextId(0) { m_AvailableIds.reserve(16); }
        IdPool(const unsigned int startId) : m_StartId(startId), m_NextId(startId) { m_AvailableIds.reserve(16); }

        unsigned int Get()
        {
            if(m_AvailableIds.empty())
            {
                return m_NextId++;
            }
            else
            {
                unsigned int id = m_AvailableIds.back();
                m_AvailableIds.pop_back();
                return id;
            }
        }

        void Free(const unsigned int id)
        {
            if(m_NextId == (id + 1))
                m_NextId--;
            else if(m_NextId > id)
                m_AvailableIds.push_back(id);
        }

        void Reset()
        {
            m_NextId = m_StartId;
            m_AvailableIds.clear();
        }

        void Reset(const unsigned int newStartId)
        {
            m_StartId = newStartId;
            m_NextId = m_StartId;
            m_AvailableIds.clear();
        }
    private:
        unsigned int m_StartId;
        unsigned int m_NextId;
        std::vector<unsigned int> m_AvailableIds;
    };
}