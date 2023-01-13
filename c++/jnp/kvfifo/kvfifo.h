#ifndef KVFIFO_H
#define KVFIFO_H

#include <list>
#include <map>
#include <memory>

template<typename K, typename V>
class kvfifo {
    private:
        using element_t = std::pair<K const &, V>;
        using queue_t = std::list<element_t>;
        using queueIteretor_t = typename queue_t::iterator;
        using itList_t = std::list<queueIteretor_t>;
        using itListPtr_t = std::shared_ptr<itList_t>;
        using itMap_t = std::map<K const &, itListPtr_t>;
        using queuePtr_t = std::shared_ptr<queue_t>;
        using itMapPtr_t = std::shared_ptr<itMap_t>;

        queuePtr_t queuePtr;
        itMapPtr_t itMapPtr;

        bool hasReference = false;

        void check_share() {
            if (!queuePtr.unique()) {
                queuePtr_t oldQueuePtr = queuePtr;
                itMapPtr_t oldItMapPtr = itMapPtr;

                try {
                    kvfifo newKvfifo;
                    for (auto elementIt: *queuePtr) {
                        std::pair<K const &, V> element = elementIt;
                        newKvfifo.push(element.first, element.second);
                    }
                    queuePtr = newKvfifo.queuePtr;
                    itMapPtr = newKvfifo.itMapPtr;

                } catch (...) {
                    queuePtr = oldQueuePtr;
                    itMapPtr = oldItMapPtr;
                    throw;
                }
            }
        }

    public:
        class k_iterator {
            public:
                using iterator_category = std::bidirectional_iterator_tag;
                using value_type = typename itMap_t::iterator; 
                using difference_type = std::ptrdiff_t;
                using pointer = K*;
                using reference = K&;

                k_iterator(value_type const p) {
                    it = p;
                }

                reference operator*() noexcept {
 		            return (const reference) it->first;
   	            }
    	        pointer operator->() noexcept {
      		        return &(it->first);
   	            }
    	        k_iterator & operator++() noexcept {  
      		        it++;
 		            return *this;
   	            }
                 k_iterator & operator--() noexcept { 
      		        it--;
 		            return *this;
   	            }
    	        k_iterator operator++(int) noexcept {
      		        k_iterator result(*this);
 		            operator++();
      		        return result;
   	            }
                k_iterator operator--(int) noexcept { 
      		        k_iterator result(*this);
 		            operator--();
      		        return result;
   	            }

    	        friend bool operator==(k_iterator const & a,
                           	   k_iterator const & b
    	        ) noexcept {
 		            return a.it == b.it;
   	            }
    	        friend bool operator!=(k_iterator const & a,
                           	   k_iterator const & b
    	        ) noexcept {
      		        return !(a == b);
   	            }
                
            private:
                value_type it;
        };

        kvfifo() {
            queuePtr = std::make_shared<queue_t>();
            itMapPtr = std::make_shared<itMap_t>();
        }
        kvfifo(kvfifo const & other) {
            queuePtr_t oldQueuePtr = queuePtr;
            itMapPtr_t oldItMapPtr = itMapPtr;

            try {
                queuePtr = other.queuePtr;
                itMapPtr = other.itMapPtr;

                if (other.hasReference) {
                    check_share();
                }
            } catch (...) {
                queuePtr = oldQueuePtr;
                itMapPtr = oldItMapPtr;
                throw;
            }
        }
        kvfifo(kvfifo && other) noexcept {
            queuePtr = std::move(other.queuePtr);
            itMapPtr = std::move(other.itMapPtr);
        }

        kvfifo & operator=(kvfifo other) {
            queuePtr_t oldQueuePtr = queuePtr;
            itMapPtr_t oldItMapPtr = itMapPtr;

            try {
                queuePtr = other.queuePtr;
                itMapPtr = other.itMapPtr;

                if (hasReference) {
                    check_share();
                }

                return *this;
            } catch (...) {
                queuePtr = oldQueuePtr;
                itMapPtr = oldItMapPtr;
                throw;
            }
        }

        void push(K const & k, V const & v) {
            check_share();

            std::pair<K const &, V> newElement(k, v);
            queue_t & queue = *queuePtr;
            itMap_t & itMap = *itMapPtr;
            queueIteretor_t newIteretor = queue.insert(queue.end(), newElement);

            try {
                if (itMap.contains(k)) {
                    itList_t & iteretorList = *(itMap.find(k)->second);
                    iteretorList.push_back(newIteretor);
                } else {
                    itList_t newIteretorList;
                    newIteretorList.push_back(newIteretor);
                    itListPtr_t newIteretorListPtr = std::make_shared<itList_t>(newIteretorList);
                    itMap.insert({k, newIteretorListPtr});
                }
                hasReference = false;
            } catch (...) {
                queue.erase(newIteretor);
                throw;
            }
        }

        void pop() {
            if (queuePtr->empty()) {
                throw std::invalid_argument("Queue is empty");
            }
            check_share();

            queue_t & queue = *queuePtr;
            itMap_t & itMap = *itMapPtr;

            K & key = queue.front().first;

            itList_t & iteretorList = *(itMap.find(key)->second);
            iteretorList.pop_front();

            if (iteretorList.empty()) {
                itMap.erase(itMap.find(key));
            }

            queue.pop_front();

            hasReference = false;
        }
        void pop(K const & k) {
            if(!itMapPtr->contains(k)){
                throw std::invalid_argument("Queue does not contain key");
            }

            check_share();

            queue_t & queue = *queuePtr;
            itMap_t & itMap = *itMapPtr;

            itList_t & iteretorList = *(itMap.find(k)->second);
            queueIteretor_t & iteretor = *(iteretorList.begin());

            queue.erase(iteretor);
            iteretorList.pop_front();

            if (iteretorList.empty()) {
                itMap.erase(itMap.find(k));
            }

            hasReference = false;
        }

        void move_to_back(K const & k) {
            if(!itMapPtr->contains(k)){
                throw std::invalid_argument("Queue does not contain key");      
            }

            check_share();

            queue_t & queue = *queuePtr;
            itMap_t & itMap = *itMapPtr;
            itList_t & iteretorList = *(itMap.find(k)->second);

            for (queueIteretor_t & it : iteretorList) {
                queue.splice(queue.end(), queue, it);
            }

            hasReference = false;
        }

        std::pair<K const &, V const &> front() const {
            if (queuePtr->empty()) {
                throw std::invalid_argument("Queue is empty");
            }

            queue_t & queue = *queuePtr;

            element_t & frontElement = queue.front();
            K const & value1 = frontElement.first;
            V const & value2 = frontElement.second;

            return std::pair<K const &, V const &>(value1, value2);
        }
        std::pair<K const &, V &> front() {
            if (queuePtr->empty()) {
                throw std::invalid_argument("Queue is empty");
            }

            check_share();

            queue_t & queue = *queuePtr;

            element_t & frontElement = queue.front();
            K const & value1 = frontElement.first;
            V & value2 = frontElement.second;

            hasReference = true;

            return std::pair<K const &, V &>(value1, value2);
        }
        std::pair<K const &, V &> back() {
            if (queuePtr->empty()) {
                throw std::invalid_argument("Queue is empty");
            }

            check_share();

            queue_t & queue = *queuePtr;

            element_t & backElement = queue.back();
            K const & value1 = backElement.first;
            V & value2 = backElement.second;

            hasReference = true;

            return std::pair<K const &, V &>(value1, value2);
        }
        std::pair<K const &, V const &> back() const {
            if (queuePtr->empty()) {
                throw std::invalid_argument("Queue is empty");
            }

            queue_t & queue = *queuePtr;

            element_t & backElement = queue.back();
            K const & value1 = backElement.first;
            V const & value2 = backElement.second;

            return std::pair<K const &, V const &>(value1, value2);
        }

        std::pair<K const &, V &> first(K const &key) {
            if (!itMapPtr->contains(key)) {
                throw std::invalid_argument("Key does not exist");
            }

            check_share();

            itMap_t & itMap = *itMapPtr;
            itList_t & iteretorList = *(itMap.find(key)->second);

            element_t & frontElement = *(iteretorList.front());
            K const & value1 = frontElement.first;
            V & value2 = frontElement.second;

            hasReference = true;

            return std::pair<K const &, V &>(value1, value2);
        }
        std::pair<K const &, V const &> first(K const &key) const {
            if (!itMapPtr->contains(key)) {
                throw std::invalid_argument("Key does not exist");
            }

            itMap_t & itMap = *itMapPtr;
            itList_t & iteretorList = *(itMap.find(key)->second);

            element_t & frontElement = *(iteretorList.front());
            K const & value1 = frontElement.first;
            V const & value2 = frontElement.second;

            return std::pair<K const &, V const &>(value1, value2);
        }
        std::pair<K const &, V &> last(K const &key) {
            if (!itMapPtr->contains(key)) {
                throw std::invalid_argument("Key does not exist");
            }

            check_share();

            itMap_t & itMap = *itMapPtr;
            itList_t & iteretorList = *(itMap.find(key)->second);

            element_t & backElement = *(iteretorList.back());
            K const & value1 = backElement.first;
            V & value2 = backElement.second;

            hasReference = true;

            return std::pair<K const &, V &>(value1, value2);
        }
        std::pair<K const &, V const &> last(K const &key) const {
            if (!itMapPtr->contains(key)) {
                throw std::invalid_argument("Key does not exist");
            }

            itMap_t & itMap = *itMapPtr;
            itList_t & iteretorList = *(itMap.find(key)->second);

            element_t & frontElement = *(iteretorList.back());
            K const & value1 = frontElement.first;
            V const & value2 = frontElement.second;

            return std::pair<K const &, V const &>(value1, value2);
        }

        size_t size() const noexcept {
            return queuePtr->size();
        }
        bool empty() const noexcept {
            return queuePtr->size() == 0;
        }

        size_t count(K const & k) const noexcept {
            auto itMapIterator = itMapPtr->find(k);
            if (itMapIterator == itMapPtr->end()) {
                return 0;
            }
            itList_t & iteretorList = *(itMapPtr->find(k)->second);
            return iteretorList.size();
        }

        void clear() {
            check_share();
            queuePtr->clear();
            itMapPtr->clear();
        }

        k_iterator k_begin() const noexcept {
            return k_iterator(itMapPtr->begin());
        }
        k_iterator k_end() const noexcept {
            return k_iterator(itMapPtr->end());
        }
        
};


#endif