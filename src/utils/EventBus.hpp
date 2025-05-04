#include <vector>
#include <functional>

template<typename Event>
class EventBus {
public:
    using Callback = std::function<void(const Event&)>;

    static void subscribe(Callback cb) {
        getSubscribers().emplace_back(std::move(cb));
    }

    static void publish(const Event& event) {
        for (const auto& cb : getSubscribers()) {
            cb(event);
        }
    }

private:
    static std::vector<Callback>& getSubscribers() {
        static std::vector<Callback> subscribers;
        return subscribers;
    }
};
