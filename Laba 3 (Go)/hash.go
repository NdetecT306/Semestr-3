package main

const HASH_SIZE = 256

type HashEntry struct {
    key       int
    value     string
    isDeleted bool
    isEmpty   bool
}

type DoubleHashTable struct {
    hashTable [HASH_SIZE]HashEntry
}

func NewDoubleHashTable() *DoubleHashTable {
    table := &DoubleHashTable{}
    table.INIT_HASH_TABLE()
    return table
}

func (dht *DoubleHashTable) INIT_HASH_TABLE() {
    for i := 0; i < HASH_SIZE; i++ {
        dht.hashTable[i] = HashEntry{
            key:       -1,
            value:     "",
            isDeleted: false,
            isEmpty:   true,
        }
    }
}

func (dht *DoubleHashTable) hash1(key int) int {
    return key % HASH_SIZE
}

func (dht *DoubleHashTable) hash2(key int) int {
    return 13 - (key % 13)
}

func (dht *DoubleHashTable) ADD(key int, value string) bool {
    index := dht.hash1(key)
    step := dht.hash2(key)
    startIndex := index
    attempts := 0

    for attempts < HASH_SIZE {
        if dht.hashTable[index].isEmpty || dht.hashTable[index].isDeleted {
            dht.hashTable[index] = HashEntry{
                key:       key,
                value:     value,
                isDeleted: false,
                isEmpty:   false,
            }
            return true
        }
        if dht.hashTable[index].key == key && !dht.hashTable[index].isDeleted {
            return false
        }
        index = (index + step) % HASH_SIZE
        attempts++
        if index == startIndex {
            break
        }
    }
    return false
}

func (dht *DoubleHashTable) SETDEL(key int, value string) bool {
    index := dht.hash1(key)
    step := dht.hash2(key)
    startIndex := index
    attempts := 0

    for attempts < HASH_SIZE {
        if dht.hashTable[index].isEmpty && !dht.hashTable[index].isDeleted {
            return false
        }
        if dht.hashTable[index].key == key && dht.hashTable[index].value == value && !dht.hashTable[index].isDeleted {
            dht.hashTable[index].isDeleted = true
            dht.hashTable[index].isEmpty = true
            return true
        }
        index = (index + step) % HASH_SIZE
        attempts++
        if index == startIndex {
            break
        }
    }
    return false
}

func (dht *DoubleHashTable) SET_AT(key int, value string) bool {
    index := dht.hash1(key)
    step := dht.hash2(key)
    startIndex := index
    attempts := 0

    for attempts < HASH_SIZE {
        if dht.hashTable[index].isEmpty && !dht.hashTable[index].isDeleted {
            return false
        }
        if dht.hashTable[index].key == key && dht.hashTable[index].value == value && !dht.hashTable[index].isDeleted {
            return true
        }
        index = (index + step) % HASH_SIZE
        attempts++
        if index == startIndex {
            break
        }
    }
    return false
}

func (dht *DoubleHashTable) GetAllElements() ([]int, []string, int) {
    keys := make([]int, 0)
    values := make([]string, 0)
    count := 0

    for i := 0; i < HASH_SIZE; i++ {
        if !dht.hashTable[i].isEmpty && !dht.hashTable[i].isDeleted {
            keys = append(keys, dht.hashTable[i].key)
            values = append(values, dht.hashTable[i].value)
            count++
        }
    }
    return keys, values, count
}

func (dht *DoubleHashTable) Size() int {
    return HASH_SIZE
}

func (dht *DoubleHashTable) ContainsKey(key int) bool {
    index := dht.hash1(key)
    step := dht.hash2(key)
    startIndex := index
    attempts := 0

    for attempts < HASH_SIZE {
        if dht.hashTable[index].isEmpty && !dht.hashTable[index].isDeleted {
            return false
        }
        if dht.hashTable[index].key == key && !dht.hashTable[index].isDeleted {
            return true
        }
        index = (index + step) % HASH_SIZE
        attempts++
        if index == startIndex {
            break
        }
    }
    return false
}

func (dht *DoubleHashTable) ElementCount() int {
    count := 0
    for i := 0; i < HASH_SIZE; i++ {
        if !dht.hashTable[i].isEmpty && !dht.hashTable[i].isDeleted {
            count++
        }
    }
    return count
}

