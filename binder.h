#pragma once
#include <functional>

class TBinder {
private:
    std::function<void()> Finalizer;
    bool Attached;

public:
    explicit TBinder(std::function<void()>  finalizer)
        : Finalizer(std::move(finalizer)), Attached(true) {
    }

    TBinder(const TBinder&) = delete;
    TBinder& operator=(const TBinder&) = delete;
    TBinder(TBinder&& src) noexcept {
        Finalizer = src.Finalizer;
        Attached = true;
        src.Attached = false;
    }

    ~TBinder() {
        Unbind();
    }

    void Unbind() {
        if (Attached) {
            Finalizer();
            Attached = false;
        }
    }
};
