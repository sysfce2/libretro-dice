// Circular Queue Template
// When an item is inserted into a full queue, the oldest item is automatically evicted.
#ifndef CIRQUE_H
#define CIRQUE_H

#define EVENT_QUEUE_SIZE 128

template <typename T, int QUEUE_SIZE = EVENT_QUEUE_SIZE> class cirque
{
private:
    //T queue[QUEUE_SIZE];
    T* queue;
    unsigned int first, next;

public:
    //cirque() : first(0), next(0) { }
    cirque() : first(0), next(0) { queue = new T[QUEUE_SIZE]; }
    ~cirque() { delete[] queue; }

    // Like an iterator
    class index
    {
    private:
        unsigned int idx;
        index(unsigned int i, bool) : idx(i) { } // Used to skip AND operation, is there a less grotesque way to do this?
    public:
        index() : idx(0) { }
        template <typename U> index(const U& u) : idx(u.idx) { }
        index(unsigned int i) : idx(i & (QUEUE_SIZE-1)) { }
        index(unsigned char c) : idx(c & (QUEUE_SIZE-1)) { }
        index(unsigned short s) : idx(s & (QUEUE_SIZE-1)) { }
        index(signed int i) : idx(i & (QUEUE_SIZE-1)) { }
        
        operator unsigned int() const { return idx; } 
                
        index& operator++()
        { 
            idx = (idx+1) & (QUEUE_SIZE-1);
            return *this; 
        }
        index operator++(int)
        { 
            index t(idx, true);
            idx = (idx+1) & (QUEUE_SIZE-1); 
            return t;
        }
        index& operator--()
        { 
            idx = (idx-1) & (QUEUE_SIZE-1);
            return *this; 
        }
        index operator--(int)
        { 
            index t(idx, true);
            idx = (idx-1) & (QUEUE_SIZE-1); 
            return t;
        }
        
        index& operator+=(int x)
        { 
            idx = (idx+x) & (QUEUE_SIZE-1);
            return *this; 
        }
        index& operator-=(int x)
        { 
            idx = (idx-x) & (QUEUE_SIZE-1);
            return *this; 
        }

        /*bool operator==(const index& i) { return idx == i.idx; }
        bool operator!=(const index& i) { return idx != i.idx; }
        index operator+(unsigned int i) { return index((idx + i) & QUEUE_SIZE, true); }
        index operator-(unsigned int i) { return index((idx - i) & QUEUE_SIZE, true); }*/

        friend class cirque<T, QUEUE_SIZE>;
        template<typename U, int S> friend class cirque<U, S>::index; 
    };

    bool empty() const
    {
        return first == next;
    }

    bool full() const
    {
        return ((next+1) & (QUEUE_SIZE-1)) == first;
    }

    uint32_t size() const
    {
        return (next-first) & (QUEUE_SIZE-1);
    }

    void push_back(const T& x)
    {
        queue[next] = x;
        next = (next+1) & (QUEUE_SIZE-1);
        
        if(next == first)
            pop_front();
    }

    void pop_front()
    {
        first = (first+1) & (QUEUE_SIZE-1);
    }

    void pop_back()
    {
        next = (next-1) & (QUEUE_SIZE-1);
    }

    void clear()
    {
        first = next;
    }

    void erase_front(const index& i) // Erase values up to, but not including, index
    {
        first = i;
    }

    void erase_back(const index& i) // Erase values after, but not including, index
    {
        next = index(i+1);
    }

    T& operator[](const index& i)
    { return queue[i.idx]; }
    
    const T& operator[](const index& i) const
    { return queue[i.idx]; }

    T& front()
    { return queue[first]; }
    
    const T& front() const
    { return queue[first]; }

    T& back()
    { return queue[(next-1) & (QUEUE_SIZE-1)]; }
    
    const T& back() const
    { return queue[(next-1) & (QUEUE_SIZE-1)]; }

    index begin() const
    { return index(first, true); }

    index end() const
    { return index(next, true); }

    // Perform binary search for element.
    // Returns index that is less than or equal to the value being searched for,
    // but where index+1 is greater than the value (or non existant).
    index binary_search(index first, index last, const T& val) const
    {
        while(first.idx != last.idx)
        {
            index mid = first + (index(unsigned(last) - first) >> 1);

            if(val < queue[mid.idx])
            {
                last = mid;
                continue;
            }

            index after = mid + 1;

            if(after == end() || val < queue[after.idx])
                return mid;

            //first = mid; // which of these is correct?
            first = after;
        }

        return first;
        
        /*if(first.idx == last.idx)
            return first;

        index mid = first + (index(unsigned(last) - first) >> 1);

        if(val < queue[mid.idx])
            return binary_search(first, mid, val);

        index after = mid + 1;
       
        if(after == end() || queue[after.idx] > val)
            return mid;

        return binary_search(mid, last, val);*/
    }
};

#endif
