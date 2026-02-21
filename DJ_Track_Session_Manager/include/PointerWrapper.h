#ifndef POINTERWRAPPER_H
#define POINTERWRAPPER_H

#include <utility>
#include <iostream>

/**
 * PointerWrapper - A template class that wraps a raw pointer
 *
 * This is Phase 3 of the assignment. Students must analyze and implement
 * a complete pointer wrapper class that manages dynamic memory safely.
 *
 * Refer to the assignment instructions (Phase 3) for detailed guiding questions
 * about resource management, ownership semantics, copy vs move, and interface design.
 */
template <typename T>
class PointerWrapper
{
private:
    T *ptr_; // Raw pointer to the managed object

public:
    // ========== CONSTRUCTION AND DESTRUCTION ==========

    /**
     * Default constructor - creates empty wrapper
     */

    /**
     * Constructor from raw pointer - wraps the pointer
     */
    explicit PointerWrapper(T *ptr = nullptr) : ptr_(ptr) {}

    /**
     * TODO: Implement destructor
     * HINT: What should happen to the wrapped pointer when the wrapper is destroyed?
     * Think about ownership and resource management.
     * Is the default destructor sufficient here?
     */
    ~PointerWrapper()
    {
        delete ptr_;
    }

    // ========== COPY OPERATIONS (DELETED) ==========

    /**
     * Copy constructor is DELETED
     * Think about why this might be necessary for a pointer wrapper
     */
    PointerWrapper(const PointerWrapper &) = delete;

    /**
     * Copy assignment is DELETED
     * Consider what problems could arise if copying was allowed
     */
    PointerWrapper &operator=(const PointerWrapper &) = delete;

    // ========== MOVE OPERATIONS (STUDENTS IMPLEMENT) ==========

    /**
     * TODO: Implement move constructor
     * HINT: How should ownership transfer from one wrapper to another?
     * What should happen to the source wrapper after the move?
     */
    PointerWrapper(PointerWrapper &&other) noexcept : ptr_(other.ptr_)
    {
        //std::cout << "Move constructor called for: " << other.ptr_ << std::endl;
        other.ptr_ = nullptr;
    }

    /**
     * TODO: Implement move assignment operator
     * HINT: Handle cleanup of current resource and ownership transfer
     * Don't forget about self-assignment!
     */
    PointerWrapper &operator=(PointerWrapper &&other) noexcept
    {
        if (this != &other)
        {
            delete ptr_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // ========== ACCESS OPERATIONS ==========

    /**
     * TODO: Implement dereference operator
     * HINT: How do you access the object that the wrapper points to? Is this operation implementation complete?
     * @throws std::runtime_error if ptr is null
     */

    T &operator*() const
    {
        return *ptr_;
    };

    /**
     * TODO: Implement arrow operator
     * HINT: How do you access members of the wrapped object?
     * What safety checks should you perform?
     */
    T *operator->() const
    {
        return ptr_;
    }

    /**
     * TODO: Implement get() function
     * HINT: Sometimes you need access to the raw pointer without changing ownership
     * What should this function return?
     * @throws std::runtime_error if ptr is null
     */
    T *get() const
    {
        return ptr_; // Placeholder
    }

    // ========== OWNERSHIP MANAGEMENT ==========

    /**
     * TODO: Implement release() function
     * HINT: What does "release" mean in terms of ownership?
     * Should the wrapper still own the pointer after calling release()?
     */
    T *release()
    {
        T *temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

    /**
     * TODO: Implement reset() function
     * HINT: How do you replace the currently wrapped pointer?
     * What should happen to the old pointer?
     */
    void reset(T *new_ptr = nullptr)
    {
        if (ptr_ != new_ptr)
        {
           // std::cout << "Reset called for pointer: " << ptr_ << std::endl;
            delete ptr_;
            ptr_ = new_ptr;
        }
    }

    // ========== UTILITY FUNCTIONS ==========

    /**
     * TODO: Implement boolean conversion operator
     * HINT: When should a wrapper be considered "true" or "false"?
     * Why might the explicit keyword be important here?
     */
    explicit operator bool() const
    {
        return ptr_ != nullptr; // placeholder
    }

    /**
     * Swap two PointerWrapper objects
     * This is implemented for you as a reference
     */
    void swap(PointerWrapper &other) noexcept
    {
        std::swap(ptr_, other.ptr_);
    }
};

// ========== NON-MEMBER FUNCTIONS ==========

/**
 * Helper function to create PointerWrapper
 * This is implemented for you as an example
 * Can you figure out when this would be useful in phase 4?
 */
template <typename T, typename... Args>
PointerWrapper<T> make_pointer_wrapper(Args &&...args)
{
    return PointerWrapper<T>(new T(std::forward<Args>(args)...));
}

/**
 * TODO: Implement swap for PointerWrapper
 * HINT: How can you swap two wrapper objects?
 * Why might this be useful?
 */
template <typename T>
void swap(PointerWrapper<T> &lhs, PointerWrapper<T> &rhs) noexcept
{
    // TODO: Implement global swap function
    // HINT: You can use the member swap function
    // your code here...
    lhs.swap(rhs);
}

#endif // POINTERWRAPPER_H
