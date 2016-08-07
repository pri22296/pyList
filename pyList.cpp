#include <iostream>
#include <stdexcept>
#include <cmath>

template<typename Type>
class pyList{
    const int INITIAL_ARRAY_SIZE = 100;
    const float RESIZE_FACTOR = 2;

    long Head;
    long Tail;
    size_t allocated;
    long length;
    Type *Array;

    pyList(const size_t);
    void _swap(pyList<Type> &);
    inline long _next_index(const long index) const { return (index + 1) % allocated; }
    inline long _prev_index(const long index) const { return (index + allocated - 1) % allocated; }
    inline long _transform_index(const long) const;
    inline long _untransform_index(const long) const;
    long _clamp(const long, const long, const long) const;
    long _len_between(const long , const long) const;
    long _partition(const long, const long);
    void _quicksort(const long, const long);
    void _testsort() const;
    bool _is_valid_index(const long) const;
    void _resize_if_necessary();

public:
    pyList();
    pyList(const pyList<Type> & );
    pyList(const std::initializer_list<Type>);
    pyList(pyList<Type> && );
    ~pyList();
    pyList<Type> & operator = (pyList<Type>);
    Type & operator [] (const long);
    const Type operator [] (const long) const;
    pyList<Type> operator + (const pyList<Type> &) const;
    void operator += (const pyList<Type> &);
    pyList<Type> operator * (const long) const;
    void operator *= (const long);
    bool operator == (const pyList<Type> &) const;
    bool operator != (const pyList<Type> &) const;
    bool operator < (const pyList<Type> &) const;
    bool operator >= (const pyList<Type> &) const;
    bool operator <= (const pyList<Type> &) const;
    bool operator > (const pyList<Type> &) const;
    inline long len() const;
    void reverse();
    Type get(const long) const;
    void set(const long, const Type);
    void append(const Type);
    void extend(const pyList<Type> &);
    pyList<Type> slice(const long, const long, const long) const;
    Type pop(const long);
    void remove(const Type);
    void clear();
    long count(const Type) const;
    long index(const Type, const long, const long);
    Type max();
    Type min();
    Type sum();
    void insert(const long, const Type);
    void sort();
};

template<typename Type>
pyList<Type>::pyList(){
    Head = 0;
    Tail = 0;
    length = 0;
    allocated = INITIAL_ARRAY_SIZE;
    Array = new Type[allocated];
}

template<typename Type>
pyList<Type>::pyList(const size_t request_size){
    Head = 0;
    Tail = 0;
    length = 0;
    allocated = request_size>INITIAL_ARRAY_SIZE?request_size:INITIAL_ARRAY_SIZE;
    Array = new Type[allocated];
}

template<typename Type>
pyList<Type>::pyList(const pyList<Type> & rhs){
    Head = 0;
    Tail = rhs.length;
    length = rhs.length;
    allocated = rhs.allocated;
    Array = new Type[allocated];
    for(unsigned long i=1; i<=Tail; i++){
        Array[i] = rhs[i-1];
    }
}

template<typename Type>
pyList<Type>::pyList(const std::initializer_list<Type> source):pyList(RESIZE_FACTOR*source.size()){
    for(Type item:source){
        Array[++Tail] = item;
    }
    length = source.size();
}

template<typename Type>
pyList<Type>::pyList(pyList<Type> && rhs){
    Head = rhs.Head;
    Tail = rhs.Tail;
    length = rhs.length;
    allocated = rhs.allocated;
    Array = rhs.Array;
    rhs.Array = nullptr;
}

template<typename Type>
pyList<Type>::~pyList(){
    delete[] Array;
}

template<typename Type>
void pyList<Type>::_swap(pyList<Type> & List){
    std::swap(Head, List.Head);
    std::swap(Tail, List.Tail);
    std::swap(length, List.length);
    std::swap(allocated, List.allocated);
    std::swap(Array, List.Array);
}

template<typename Type>
pyList<Type> & pyList<Type>::operator = (pyList<Type> rhs){
    this->_swap(rhs);
    return *this;
}

template<typename Type>
Type & pyList<Type>::operator [] (const long index){
    long act_index = _transform_index(index);
    return Array[act_index];
}

template<typename Type>
const Type pyList<Type>::operator [] (const long index) const {
    long act_index = _transform_index(index);
    return Array[act_index];
}

template<typename Type>
bool pyList<Type>::operator == (const pyList<Type> & rhs) const {
    if(length != rhs.length){ return false; }
    if(this != &rhs){
        for(unsigned long i=0; i<length; i++){
            if((*this)[i] != rhs[i]){ return false; }
        }
    }
    return true;
}

template<typename Type>
bool pyList<Type>::operator != (const pyList<Type> & rhs) const {
    return !((*this) == rhs);
}

template<typename Type>
bool pyList<Type>::operator < (const pyList<Type> & rhs) const {
    long min_len = length < rhs.length ? length : rhs.length;
    for(unsigned long i=0; i<min_len; ++i){
        if((*this)[i] > rhs[i]){
            return false;
        }
        else if((*this)[i] < rhs[i]){
            return true;
        }
    }
    if(length < rhs.length){
        return true;
    }
    return false;
}

template<typename Type>
bool pyList<Type>::operator >= (const pyList<Type> & rhs) const {
    return !((*this) < rhs);
}

template<typename Type>
bool pyList<Type>::operator <= (const pyList<Type> & rhs) const {
    return !((*this) > rhs);
}

template<typename Type>
bool pyList<Type>::operator > (const pyList<Type> & rhs) const {
    return (rhs < (*this));
}

template<typename Type>
pyList<Type> pyList<Type>::operator + (const pyList<Type> & rhs) const {
    pyList<Type> temp = *this;
    temp.extend(rhs);
    return temp;
}

template<typename Type>
void pyList<Type>::operator += (const pyList<Type> & rhs){
    this->extend(rhs);
}

template<typename Type>
pyList<Type> pyList<Type>::operator * (const long rhs) const {
    pyList<Type> temp;
    for(long i=0; i<rhs; i++){
        temp.extend(*this);
    }
    return temp;
}

template<typename Type>
void pyList<Type>::operator *= (const long rhs){
    pyList<Type> temp;
    if(rhs <= 0){
        *this = temp;
    }
    temp = *this;
    for(long i=1; i<rhs; i++){
        this->extend(temp);
    }
}

template<typename Type>
const pyList<Type> operator * (const long lhs, pyList<Type> & rhs){
    return (rhs*lhs);
}

template<typename Type>
std::ostream & operator << (std::ostream & o, const pyList<Type> & List){
    o << "[";
    int flag = 0;
    long n = List.len();
    for(long i=0; i < n; i++){
        if(flag == 0){ o << List[i] ; ++flag;}
        else{ o << ", " << List[i]; }
    }
    return o << "]";
}

template<typename Type>
inline long pyList<Type>::_transform_index(const long index) const{
    long act_index = (index>=0) ? (index) : (index + length);
    act_index = (act_index + Head + 1) % allocated;
    return act_index;
}

template<typename Type>
inline long pyList<Type>::_untransform_index(const long index) const{
    if(Head < Tail){
        return index - Head - 1;
    }
    else {
        if(index > Head){
            return index - Head - 1;
        }
        else{
            return index + allocated - Head - 1;
        }
    }
}

template<typename Type>
long pyList<Type>::_clamp(const long value, const long min_value, const long max_value) const {
    if(value < min_value){
        return min_value;
    }
    else if(value > max_value){
        return max_value;
    }
    return value;
}

template<typename Type>
long pyList<Type>::_len_between(const long start, const long stop) const {
    if(start <= stop){
        return stop - start;
    }
    else{
        return allocated - start + stop;
    }
}

template<typename Type>
bool pyList<Type>::_is_valid_index(const long index) const {
    long k = _untransform_index(index);
    if(k<0 || k>=length){
        return false;
    }
    else{
        return true;
    }
}

template<typename Type>
void pyList<Type>::_resize_if_necessary(){
    size_t newsize;
    if(length >= allocated-1){
        newsize = RESIZE_FACTOR*allocated;
    }
    else if(length < allocated/(RESIZE_FACTOR*RESIZE_FACTOR) && allocated/RESIZE_FACTOR > INITIAL_ARRAY_SIZE){
        newsize = allocated/RESIZE_FACTOR;
    }
    else{
        return;
    }
    Type *temp = new Type[newsize];
    long index = Head;
    temp[0] = Array[Head];
    for(unsigned long i=0; i< length; i++){
        index = _next_index(index);
        temp[i+1] = Array[index];
    }
    Head = 0;
    Tail = length;
    delete[] Array;
    Array = temp;
    allocated = newsize;
}

template<typename Type>
long pyList<Type>::_partition(const long start, const long stop){
    long i=start;
    long j=stop;
    while(1){
        int all_elements_equal_flag = 1;
        while(i != _next_index(stop)  && Array[i] <= Array[start]){
            if(Array[i] != Array[start]){
                all_elements_equal_flag = 0;
            }
            i = _next_index(i);
        }
        if(all_elements_equal_flag == 1 && i == _next_index(stop)){
            return -1;
        }
        while(j != start && Array[j] > Array[start]){
            j = _prev_index(j);
        }
        if(_next_index(j) == i){
            break;
        }
        std::swap(Array[i], Array[j]);
    }
    std::swap(Array[start], Array[j]);
    return j;
}

template<typename Type>
void pyList<Type>::_quicksort(const long start, const long stop){
    if(start == stop){
        return;
    }
    if(_len_between(start, stop) <= 64){
        for(long i=_next_index(start); i!=_next_index(stop); i = _next_index(i)){
            Type temp = Array[i];
            long j = _prev_index(i);
            long k = _next_index(j);
            while(j != _prev_index(start) && (Array[j] > temp)){
                std::swap(Array[k], Array[j]);
                k = j;
                j = _prev_index(j);
            }
            Array[k] = temp;
        }
        return;
    }
    long mid_index;
    if(start < stop){
        mid_index = (start + stop)/2;
        //mid_index = _py_median_of_index(List->Array, start + rand()%(stop-start+1), start + rand()%(stop-start+1), start + rand()%(stop-start+1));
    }
    else{
        mid_index = (start + (allocated - start + stop)/2 )% allocated;
        //mid_index = _py_median_of_index(List->Array,(start + (rand()%(List->allocated - start + stop+1)))%List->allocated,
                                        //(start + (rand()%(List->allocated - start + stop+1)))%List->allocated,
                                        //(start + (rand()%(List->allocated - start + stop+1)))%List->allocated);
    }
    std::swap(Array[start], Array[mid_index]);
    long p = _partition(start,stop);
    if(p == -1){
        return;
    }
    if(p != start){
        _quicksort(start,_prev_index(p));
    }
    if(p != stop){
        _quicksort(_next_index(p),stop);
    }
}

template<typename Type>
void pyList<Type>::_testsort() const {
    Type prev = (*this)[0];
    for(unsigned long i=1; i < length; i++){
        if((*this)[i]< prev){
            std::cerr << "Incorrect sort" << std::endl;
        }
        prev = (*this)[i];
    }
    std::cout << "Correct sort" << std::endl;
}

template<typename Type>
inline long pyList<Type>::len() const{
    return length;
}

template<typename Type>
void pyList<Type>::sort(){
    if(length <= 1){
        return;
    }
    _quicksort(_next_index(Head), Tail);
}

template<typename Type>
void pyList<Type>::reverse(){
    long k = length/2;
    long index1 = _next_index(Head);
    long index2 = Tail;
    for(unsigned long i=0; i<k; i++){
        std::swap(Array[index1], Array[index2]);
        index1 = _next_index(index1);
        index2 = _prev_index(index2);
    }
}

template<typename Type>
Type pyList<Type>::get(const long index) const {
    if(index < length && index >= -length){
        long act_index = _transform_index(index);
        return Array[act_index];
    }
    else{
        throw std::invalid_argument("IndexError: list index out of range");
    }
}

template<typename Type>
void pyList<Type>::set(const long index, const Type key){
    if(index < length && index >= -length){
        long act_index = _transform_index(index);
        Array[act_index] = key;
    }
    else{
        throw std::invalid_argument("list index out of range");
    }
}

template<typename Type>
void pyList<Type>::append(const Type key){
    _resize_if_necessary();
    Tail = (Tail + 1) % allocated;
    ++length;
    Array[Tail] = key;
}

template<typename Type>
Type pyList<Type>::max(){
    if(length == 0){
        throw std::domain_error("max of empty list");
    }
    Type maximum = (*this)[0];
    long index1 = Head;
    while(index1 != Tail){
        index1 = _next_index(index1);
        if(Array[index1] > maximum){
            maximum = Array[index1];
        }
    }
    return maximum;
}

template<typename Type>
Type pyList<Type>::min(){
    if(length == 0){
        throw std::domain_error("min of empty list");
    }
    Type minimum = (*this)[0];
    long index1 = Head;
    while(index1 != Tail){
        index1 = _next_index(index1);
        if(Array[index1] < minimum){
            minimum = Array[index1];
        }
    }
    return minimum;
}

template<typename Type>
Type pyList<Type>::sum(){
    Type total = 0;
    long index1 = Head;
    while(index1 != Tail){
        index1 = _next_index(index1);
        total = total + Array[index1];
    }
    return total;
}

template<typename Type>
void pyList<Type>::extend(const pyList<Type> & List){
    long index = List.Head;
    for(unsigned long i=0; i<List.length; i++){
        _resize_if_necessary();
        Tail = _next_index(Tail);
        index = List._next_index(index);
        Array[Tail] = List.Array[index];
    }
    length += List.length;
}

template<typename Type>
pyList<Type> pyList<Type>::slice(const long start = 0, const long stop = LONG_MAX, const long step = 1) const {
    if(step == 0){
        throw std::invalid_argument("step cannot be zero");
    }
    long stop_clamped = _clamp(stop,-length-1,length);
    stop_clamped = stop_clamped>=0? stop_clamped:stop_clamped + length;
    long start_clamped = _clamp(start,-length, length-1);
    start_clamped = start_clamped>=0? start_clamped: start_clamped + length;
    pyList newList((std::abs(stop_clamped - start_clamped/step)+1)*2);
    newList.Head = 0;
    long ctr = 0;
    long step_sgn = step/std::abs(step);
    for(long i = start_clamped; step_sgn*i < step_sgn*stop_clamped ; i += step){
        newList.Array[++ctr] = Array[_transform_index(i)];
    }
    newList.Tail = ctr;
    newList.length = ctr;
    return newList;
}

template<typename Type>
Type pyList<Type>::pop(const long index = -1){
    if(index<length && -index<=length){
        long act_index = _transform_index(index);
        Type key = Array[act_index];
        if(_len_between(act_index,Tail) < length/2){
            while(act_index != Tail){
                long next_index = _next_index(act_index);
                Array[act_index] = Array[next_index];
                act_index = next_index;
            }
            Tail = _prev_index(Tail);
        }
        else{
            long head_next = _next_index(Head);
            while(act_index != head_next){
                long prev_index = _prev_index(act_index);
                Array[act_index] = Array[prev_index];
                act_index = prev_index;
            }
            Head = head_next;
        }
        _resize_if_necessary();
        --length;
        return key;
    }
    else{
        throw std::invalid_argument("pop index out of range");
    }
}

template<typename Type>
void pyList<Type>::remove(const Type key){
    long index = Head;
    while(index != Tail){
        index = _next_index(index);
        if(Array[index] == key){
            index = _untransform_index(index);
            pop(index);
            return;
        }
    }
    throw std::invalid_argument("Value not in list");
}

template<typename Type>
void pyList<Type>::clear(){
    delete[] Array;
    Head = 0;
    Tail = 0;
    length = 0;
    allocated = INITIAL_ARRAY_SIZE;
    Array = new Type[allocated];
}

template<typename Type>
long pyList<Type>::count(const Type key) const{
    long index1 = Head;
    long ctr = 0;
    while(index1 != Tail){
        index1 = _next_index(index1);
        if(Array[index1] == key){
            ctr++;
        }
    }
    return ctr;
}

template<typename Type>
long pyList<Type>::index(const Type key, const long start = 0, const long stop = LONG_MAX){

    long stop_clamped = _clamp(stop,-length-1,length);
    stop_clamped = stop_clamped>=0? stop_clamped:stop_clamped + length;
    long start_clamped = _clamp(start,-length, length-1);
    start_clamped = start_clamped>=0? start_clamped: start_clamped + length;

    long index1 = _transform_index(start_clamped);
    long index2 = _transform_index(stop_clamped);

    while(index1 != index2){
        if(!_is_valid_index(index1)){
            break;
        }
        if(Array[index1] == key){
            return _untransform_index(index1);
        }
        index1 = _next_index(index1);
    }
    throw std::invalid_argument("Value not in list");
}

template<typename Type>
void pyList<Type>::insert(const long index, const Type key){
    _resize_if_necessary();
    long act_index = _transform_index(_clamp(index,-length-1,length));
    if(_len_between(act_index, Tail) < length/2){
        Tail = _next_index(Tail);
        long i = Tail;
        while(i != act_index){
            long prev_index = _prev_index(i);
            Array[i] = Array[prev_index];
            i = prev_index;
        }
        Array[act_index] = key;
        ++length;
    }
    else{
        long i = Head;
        Head = _prev_index(Head);
        while(i != _prev_index(act_index)){
            long next_index = _next_index(i);
            Array[i] = Array[next_index];
            i = next_index;
        }
        Array[i] = key;
        ++length;
    }
}

/*int main(){
    using namespace std;
    pyList<float> List;
    for(int i=0; i<10 ;i++){
        List.append(i+0.3);
    }
    cout << List << endl;
    cout << List.sum() << endl;
    pyList<float> List1;
    List1.append(9.9);
    List1.extend(List);
    cout << List1.max() << endl;
    cout << List1 << endl;
    pyList<string> List2;
    List2.append("b");
    List2.append("a");
    cout << List2 << endl;
    cout << List2.min() << endl;
    cout << List2.max() << endl;
    List2.sort();
    cout << List2 << endl;
    return 0;
}*/

/*int main(){
    using namespace std;
    pyList<int> List = {5,7,2};
    cout << List << endl;
    List.insert(2,21);
    cout << List << endl;
    List.insert(1,7);
    cout << "length is " << List.len() << endl;
    List[0] = 67;
    cout << List << endl;
    List.reverse();
    cout << List << endl;
    cout << "value at index 3 is " << List[3] <<endl;
    List.reverse();
    cout << List << endl;
    List.sort();
    cout << List << endl;
    pyList<int> List1 = List;
    cout << "popped value at index 0 is " << List1.pop(0) <<endl;
    cout << List1 << endl;
    cout << "min value is " << List1.min() <<endl;
    cout << "max value is " << List1.max() <<endl;
    cout << List << endl;
    cout << "sum is " << List.sum() <<endl;
    cout << List << endl;
    cout << "max value is " << List.max() <<endl;
    cout << "popped value is " << List.pop() <<endl;
    cout << List << endl;
    List.remove(7);
    cout << List << endl;
    cout << "min value is " << List1.min() << endl;
    cout << "max value is " << List1.max() << endl;
    cout << "count of 7 is " << List.count(7) << endl;
    cout << "index of 21 is " << List.index(21) << endl;
    cout << "index of 21 is " << List.index(21,0,3) <<endl;
    cout << "index of 7 is " << List.index(7,1,23) << endl;
    cout << "index of 7 is " << List.index(7,1,2) << endl;
    cout << "length is " << List.len() << endl;
    List.clear();
    cout << List << endl;
    cout << List1 << endl;
    List.extend(List1);
    cout << List << endl;
    List[1] = 12;
    cout << List << endl;
    cout << (3*List) << endl;
    cout << (List*3).slice(-71,-1,2) << endl;
    cout << (2*List == List*2) << endl;
    return 0;
}*/

int main(){
    using namespace std;
    pyList<pyList<int>> p = {{1,2,3},{3,4,5,7,8}};
    cout << p << endl;
}

