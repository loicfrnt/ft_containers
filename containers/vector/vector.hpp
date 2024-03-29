#ifndef VECTOR_HPP
# define VECTOR_HPP

# include <memory> // needed for std::allocator
# include <limits> // needed for vector::max_size()

// type_traits, contains iterator_traits, enable_if, is_integral
# include "../utils/type_traits.hpp"
// iterators, contains reverse_iterator
# include "../utils/comparisons.hpp"
// iterators, contains reverse_iterator
# include "../utils/iterators.hpp"
// iterators, specific to vec
# include "vec_iterator.hpp"

// needed for macro below
#include <sstream>
// macro for fast string_streams
# define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

// macro for capacity increse
# define NEWCP (_sz == 0 ? 1 : _sz * 2)

namespace ft
{

template <class T, class Alloc = std::allocator<T> > class vector
{
public:

    /*
     * **************************************
     * ********** Member Types **************
     * **************************************
    */

    typedef T       value_type;
    typedef Alloc   allocator_type;

    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;

    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;

    /*
     * **************************************
     * ********* Iterator Types *************
     * **************************************
    */

    typedef vec_iterator<value_type>        iterator;
    typedef vec_iterator<const value_type>   const_iterator;

    typedef reverse_iterator<const_iterator>       const_reverse_iterator;
    typedef reverse_iterator<iterator>             reverse_iterator;



private:

    /*
     * **************************************
     * ************ Variables ***************
     * **************************************
    */

    allocator_type  _al;    // Allocator
    pointer         _ar;    // Underlying array
    size_type       _sz;    // this.size()
    size_type       _cp;    // this.capacity()

    /*
     * **************************************
     * ********* Private Functions **********
     * **************************************
     *
     * These are used to avoid rewriting code
    */

    // Destroys and deallocate all vector content
    void empty_self() {
        for (unsigned int i = 0; i < _sz; i++)
            _al.destroy(_ar + i);
        if (_cp)
            _al.deallocate(_ar, _cp);
    }

    void realloc_self(size_type new_cp) {
        pointer old_ar = _ar;

        _ar = _al.allocate(new_cp);
        for (size_type i = 0; i < _sz; i++)
            _al.construct(_ar + i, *(old_ar + i));

        for (size_type i = 0; i < _sz; i++)
            _al.destroy(old_ar + i);
        _al.deallocate(old_ar, _cp);

        _cp = new_cp;
    }

    template <class Ite>
    size_type _range(Ite first, Ite last){
        size_type ret = 0;
        while (first != last){
            ++first;
            ++ret;
        }
        return ret;
    }

public:

    /*
     * **************************************
     * ********* Member Functions ***********
     * **************************************
    */

    // ***** Constructors *****

    // Constructs an empty container with the given allocator alloc
    explicit vector(const allocator_type& alloc = allocator_type()):
        _al(alloc),
        _ar(0), _sz(0), _cp(0){}

    // Fill
    // Constructs a container with n elements.
    // Each element is a copy of val
    explicit vector(size_type n,
                    const value_type& val = value_type(),
                    const allocator_type& alloc = allocator_type()):
        _al(alloc),
        _ar(0),
        _sz(n),
        _cp(n)
    {
        if (n)
            _ar = _al.allocate(n);
        else
            _ar = 0;
        for (unsigned int i = 0; i < _sz; i++)
            _al.construct(_ar + i, val);
    }

    // Range:
    // Constructs a container with as many elements as the range [first,last),
    // with each element constructed from its corresponding element in that range,
    // in the same order.
    template <class InputIt>
    vector(typename ft::enable_if<!is_integral<InputIt>::value, InputIt>::type first, InputIt last,
            const allocator_type& alloc = allocator_type()):
        _al(alloc),
        _ar(0),
        _sz(0),
        _cp(0)
    {
        insert(end(), first, last);
    }

    // Copy:
    vector (const vector& cpy):
        _al(allocator_type()),
        _ar(0),
        _sz(0),
        _cp(0)
    {
        assign(cpy.begin(), cpy.end());
    }

    // ***** Destructor *****
    ~vector(){ empty_self(); }

    // ***** Assignment operator *****
    vector& operator=(const vector& cpy) {
        assign(cpy.begin(), cpy.end());
        return *this;
    }


    /*
     * **************************************
     * ************ Iterators ***************
     * **************************************
    */

    iterator begin() { iterator ret(_ar); return ret; }
    const_iterator begin() const { const_iterator ret(_ar); return ret; }

    reverse_iterator rbegin() { reverse_iterator ret(end()); return ret; }
    const_reverse_iterator rbegin() const { const_reverse_iterator ret(end()); return ret; }

    iterator end() { iterator ret(_ar + _sz); return ret; }
    const_iterator end() const { const_iterator ret(_ar + _sz); return ret; }

    reverse_iterator rend() { reverse_iterator ret(_ar); return ret; }
    const_reverse_iterator rend() const { const_reverse_iterator ret(_ar); return ret; }


    /*
     * **************************************
     * ************* Capacity ***************
     * **************************************
    */

    size_type size() const { return _sz; }

    // This is ugly but basicly it just divides max memory by size of T, and make sure it's above 1
    size_type max_size() const {
        return std::numeric_limits<difference_type>::max() / (sizeof(value_type) / 2 < 1 ? 1 : sizeof(value_type) / 2);
    }

    size_type capacity() const { return _cp; }

    bool empty() const { return !_sz  ? true : false; }

    // Resizes the container to contain n elements.
    // - If the current size is greater than n,
    //   the container is reduced to its first count elements.
    // - If the current size is less than n,
    //   additional copies of val are inserted
    void resize (size_type n, value_type val = value_type()) {
        if (n < _sz)
            for (size_type i = n; i < _sz; i++)
                _al.destroy(_ar + i);
        else if (_sz < n)
            insert(end(), n - _sz, val);
        _sz = n;
    }


    void reserve(size_type n){
        if (n > max_size())
            throw std::length_error("vector::reserve");
        if (n > _cp)
            realloc_self(n);
    }


    /*
     * **************************************
     * ********** Element Access ************
     * **************************************
    */

    // Get allocator
    allocator_type get_allocator() const { return _al;}

    // [] operator
    reference       operator[](size_type pos)       { return *(_ar + pos);}
    const_reference operator[](size_type pos) const { return *(_ar + pos);}

private:
    // Used to throw the same exception
    // as std::vector when trying at() out of bounds
    void _at_range_check(size_type n) const {
        if ( n >= _sz)
            throw (std::out_of_range(SSTR("vector::_M_range_check: __n (which is "
                   << n << ") >= this->size() (which is " << _sz << ')')));
    }
public:
    reference       at(size_type pos)       { _at_range_check(pos); return *(_ar + pos);}
    const_reference at(size_type pos) const { _at_range_check(pos); return *(_ar + pos);}

    reference       front()       { return *_ar;}
    const_reference front() const { return *_ar;}


    reference       back()       { return *(_ar + _sz - 1);}
    const_reference back() const { return *(_ar + _sz - 1);}

    /*
     * **************************************
     * ************ Modifiers ***************
     * **************************************
    */

    // Push back
    void push_back (const value_type& val){
        if (_sz >= _cp) {
            pointer old_ar = _ar;
            _ar = _al.allocate(NEWCP);
            for (size_type i = 0; i < _sz; i++)
                _al.construct(_ar + i, *(old_ar + i));
            _al.construct(_ar + _sz, val);
            for (size_type i = 0; i < _sz; i++)
                _al.destroy(old_ar + i);
            if (old_ar)
                _al.deallocate(old_ar, _cp);
            _cp = NEWCP;
        }
        else
            _al.construct(_ar + _sz, val);
        ++_sz;
    }

    // Pop Back
    void pop_back() { _al.destroy(_ar + --_sz); }


    // Inserts value before pos
    iterator insert( iterator pos, const T& value ) {
        size_type goal = pos - begin();

        if (!_sz && _cp)
            _al.construct(_ar + goal, value);
        else if (_sz < _cp) {
            size_type i = _sz;
            _al.construct(_ar + i, *(_ar + i - 1));
            i--;
            while (i > goal) {
                *(_ar + i) = *(_ar + i - 1);
                i--;
            }
            *(_ar + goal) = value;
        }
        else {
            pointer xar = _ar;
            _ar = _al.allocate(NEWCP);
            size_type i = 0;
            while (i < goal) {
                _al.construct(_ar + i, *(xar + i));
                i++;
            }
            _al.construct(_ar + i, value);
            while (i < _sz) {
                 _al.construct(_ar + i + 1, *(xar + i));
                 i++;
            }
            while (i > 0)
                _al.destroy(xar + --i);
            if (_cp)
                _al.deallocate(xar, _cp);
            _cp = NEWCP;
        }
        _sz++;
        return iterator(_ar + goal);
    }
    // Inserts count copies of the value before pos
    void insert( iterator pos, size_type count, const T& value ) {
        if (count == 0)
            return;
        if (count == 1)
            return (void) insert(pos, value);
        size_type new_sz = _sz + count;
        if (!_sz && _cp)
        {
            size_type i = 0;
            while (i < new_sz)
               _al.construct(_ar + i++, value);
        }
        else if (new_sz < _cp)
            _insert_count_noalloc(pos, count, value);
        else
            _insert_count_realloc(pos, count, value);
        _sz = new_sz;
    }

private:
    // insert(count) private ways 1/2
    void _insert_count_noalloc( iterator pos, size_type count, const T& value ){
        size_type goal = pos - begin();
        size_type new_sz = _sz + count;
        size_type i = 1;
        while (new_sz - i >= _sz && _sz - i >= goal) {
            _al.construct(_ar + new_sz - i, _ar[_sz - i]);
            ++i;
        }
        while (new_sz - i >= _sz) {
            _al.construct(_ar + new_sz - i, value);
            ++i;
        }
        while (new_sz - i >= goal && new_sz - i >= goal + count) {
            _ar[new_sz - i] = _ar[_sz - i];
            if (!(new_sz - i))
                break;
            ++i;
        }
        while (new_sz - i >= goal) {
            _ar[new_sz - i] = value;
            if (!(new_sz - i))
                break;
            ++i;
        }
    }

    // insert(count) private ways 2/2
    void _insert_count_realloc( iterator pos, size_type count, const T& value ){
        size_type goal = pos - begin();
        size_type new_sz = _sz + count;
        size_type i = 0;
        pointer old_ar = _ar;
        _ar = _al.allocate(std::max(NEWCP, new_sz));
        while (i < goal) {
            _al.construct(_ar + i, *(old_ar + i));
            i++;
        }
        while (i < goal + count)
            _al.construct(_ar + i++, value);
        while (i < new_sz) {
            _al.construct(_ar + i, *(old_ar + i - count));
            i++;
        }
        for (i = 0; i < _sz; i++)
            _al.destroy(old_ar + i);
        if (_cp)
            _al.deallocate(old_ar, _cp);
        _cp = std::max( NEWCP, new_sz);
    }

public:
    // Insert all the element between first and last at pos
    // enable_if is_integral is used to differenciate with insert(pos, count, val)
    //
    // Then, one of the two private spicialization declared below will be used,
    // depending on wether iterator_category being a pure input_iterator,
    // or a more complete one (at least forward_iterator)
    template <class InputIt>
    void insert(iterator pos,
                typename ft::enable_if<!is_integral<InputIt>::value, InputIt>::type first,
                InputIt last)
    {
        _insert_pv(pos, first, last,
                  typename iterator_traits<InputIt>::iterator_category());
    }

private:
    // Only an input_iterator, inserts element one by one,
    // reallocating each time size is equal to capacity.
    // this is bcause input_iterators can only be read and incremented once
    template <class InputIt>
    void _insert_pv(iterator pos, InputIt first, InputIt last, std::input_iterator_tag) {
        if (!_cp)
            while (first != last) {
                pos = insert(pos, *first);
                ++first;
                ++pos;
            }
        else {
            vector tmp(first, last);
            insert(pos, tmp.begin(), tmp.end());
        }
    }

    // insert(range) private ways 1/2
    template <class InputIt>
    void _insert_input_noalloc(iterator pos, InputIt first, InputIt last){
        size_type goal = pos - begin();
        size_type range = _range(first, last);
        size_type new_sz = _sz + range;
        size_type i = 1;

        while (new_sz - i >= _sz && _sz - i >= goal) {
            _al.construct(_ar + new_sz - i, *(_ar +_sz - i));
            ++i;
        }
        while (new_sz - i >= _sz) {
            _al.construct(_ar + new_sz - i, value_type());
            ++i;
        }
        while (new_sz - i >= goal + range) {
            _ar[new_sz - i] = *(_ar + _sz - i);
            ++i;
        }
        while (first != last)
            *(_ar + goal++) = *first++;
    }

    // insert(range) private ways 2/2
    template <class InputIt>
    void _insert_input_realloc(iterator pos, InputIt first, InputIt last){
        size_type goal = pos - begin();
        size_type range = _range(first, last);
        size_type new_sz = _sz + range;
        size_type i = 0;
        pointer old_ar = _ar;

        _ar = _al.allocate(std::max(NEWCP, new_sz));
        while (i < goal) {
            _al.construct(_ar + i, *(old_ar + i));
            i++;
        }
        while (i < goal + range)
            _al.construct(_ar + i++, *first++);
        while (i < new_sz) {
            _al.construct(_ar + i, *(old_ar + i - range));
            i++;
        }
        for (i = 0; i < _sz; i++)
            _al.destroy(old_ar + i);
        if (_cp)
            _al.deallocate(old_ar, _cp);
        _cp = std::max(NEWCP, new_sz);
}

    // if InputIt is at least a forward_iterator, optimization is possible,
    // by knowing the new capacity before insertion
    template <class InputIt>
    void _insert_pv(iterator pos, InputIt first, InputIt last, std::forward_iterator_tag) {
        size_type range = _range(first, last);
        size_type new_sz = _sz + range;

        if (range == 0)
            return;
        if (new_sz < _cp)
            _insert_input_noalloc(pos, first, last);
        else
            _insert_input_realloc(pos, first, last);
        _sz = new_sz;
    }

    // *** end of insert *** //

public:

    // does the equivalent of rebuilding the vector with vector(count, value)
    // (but optimized to reuse previously constructed elements)
    void assign( size_type count, const T& value ) {
        if (count > _cp) {
            pointer next = _al.allocate(count);
            for (size_type i = 0; i < count; i++)
                _al.construct(next + i, value);
            empty_self();
            _cp = count;
            _ar = next;
        }
        else {
            for (size_type i = 0; i < _sz; i++)
                *(_ar + i) = value;
            if (_sz > count)
                while (count != _sz)
                    _al.destroy(_ar + --_sz);
            else
                while (count != _sz)
                    _al.construct(_ar + _sz++, value);
        }
        _sz = count;
    }

    // Same as insert, see comment right above for details
    template< class InputIt >
    void assign( typename ft::enable_if<!is_integral<InputIt>::value, InputIt>::type first, InputIt last )
    {
        _assign_pv(first, last,
                  typename iterator_traits<InputIt>::iterator_category());
    }

private:
    template <class InputIt>
    void _assign_pv(InputIt first, InputIt last, std::input_iterator_tag) {
        size_type i = 0;
        while (i < _sz && first != last)
            *(_ar + i++) = *first++;
        if (first == last){
            while (i < _sz)
                _al.destroy(_ar + --_sz);
        }
        else while (first != last)
            insert(end(), *first++);
    }

    template <class InputIt>
    void _assign_pv(InputIt first, InputIt last, std::forward_iterator_tag) {
        size_type new_sz = _range(first, last);
        if (new_sz > _cp) {
            pointer next = _al.allocate(new_sz);
            for (size_type i = 0; i < new_sz; i++)
                _al.construct(next + i, *first++);
            empty_self();
            _cp = new_sz;
            _ar = next;
        }
        else {
            for (size_type i = 0; i < _sz && i < new_sz; i++)
                *(_ar + i) = *first++;
            if (_sz > new_sz)
                while (new_sz != _sz)
                    _al.destroy(_ar + --_sz);
            else
                while (new_sz != _sz)
                    _al.construct(_ar + _sz++, *first++);
        }
        _sz = new_sz;
    }
    // *** end of assign ***/

public:
    // removes the element at pos
    iterator erase( iterator pos ) {
        size_type ptr = pos - begin();
        while (ptr < _sz - 1) {
            *(_ar + ptr) = *(_ar + ptr + 1);
            ptr++;
        }
        _al.destroy(_ar + --_sz);
        return pos;
    }

    // removes the elements in range [first, last)
    iterator erase( iterator first, iterator last) {
        size_type range = last - first;
        iterator  first_cpy = first;
        while (last != end()) {
            *first = *last;
            ++first;
            ++last;
        }
        while (range) {
            _al.destroy(_ar + --_sz);
            --range;
        }
        return first_cpy;
    }

    void swap(vector &x) {
        pointer    tmp_ar = x._ar;
        size_type  tmp_sz = x._sz;
        size_type  tmp_cp = x._cp;

        x._ar = _ar;
        x._sz = _sz;
        x._cp = _cp;

        _ar = tmp_ar;
        _sz = tmp_sz;
        _cp = tmp_cp;
    }

    void clear() {
        for (size_type i = 0; i < _sz; i++)
            _al.destroy(_ar + i);
        _sz = 0;
    }


};


/*
 * **************************************
 * ********** Relational Ope ************
 * **************************************
*/

// equality
template <class T, class Alloc>
bool operator==(const vector<T, Alloc>& lhs,
                const vector<T, Alloc>& rhs) {
    if (lhs.size() != rhs.size())
        return false;
    typename vector<T, Alloc>::const_iterator lit = lhs.begin();
    typename vector<T, Alloc>::const_iterator rit = rhs.begin();
    while (lit != lhs.end())
        if (*lit++ != *rit++)
            return false;
    return true;
}

template <class T, class Alloc>
bool operator!=(const vector<T, Alloc>& lhs,
                const vector<T, Alloc>& rhs) {
    return !(lhs == rhs);
}

template <class T, class Alloc>
bool operator<(const vector<T, Alloc>& lhs,
                const vector<T, Alloc>& rhs) {
    return ft::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

template <class T, class Alloc>
bool operator>(const vector<T, Alloc>& lhs,
                const vector<T, Alloc>& rhs) {
    return (!(lhs == rhs) && !(lhs < rhs));
}

template <class T, class Alloc>
bool operator<=(const vector<T, Alloc>& lhs,
                const vector<T, Alloc>& rhs) {
    return ((lhs == rhs) || (lhs < rhs));
}

template <class T, class Alloc>
bool operator>=(const vector<T, Alloc>& lhs,
                const vector<T, Alloc>& rhs) {
    return ((lhs == rhs) || !(lhs < rhs));
}

}

# undef NEWCP
#endif
