use std::ptr;
struct Numbers {
    num: *mut String,
    size: usize,
    capacity: usize,
}
impl Numbers {
    fn new(capacity: usize) -> Self {
        let layout = std::alloc::Layout::array::<String>(capacity)
            .expect("Failed to create memory layout");
        let ptr = unsafe { std::alloc::alloc(layout) as *mut String };
        Self {
            num: ptr,
            size: 0,
            capacity,
        }
    }
    fn add_at_index(&mut self, index: usize, value: String) {
    if index >= self.capacity {
        println!("Индекс вне диапазона емкости массива.");
        return;
    }
    if index >= self.size {
        for i in self.size..index {
            unsafe {
                ptr::write(self.num.add(i), String::new());
            }
        }
        unsafe {
            ptr::write(self.num.add(index), value);
        }
        self.size = index + 1;
    } else {
        unsafe {
            // Заменяем элемент, правильно обрабатывая старый
            let old_value = ptr::replace(self.num.add(index), value);
            drop(old_value); // Явно освобождаем старую строку
        }
    }
}
    fn add_at_end(&mut self, value: String) {
        if self.size >= self.capacity {
            println!("Массив заполнен.");
            return;
        }
        unsafe {
            ptr::write(self.num.add(self.size), value);
        }
        self.size += 1;
    }
    fn get_at_index(&self, index: usize) -> Option<&str> {
        if index < self.size {
            unsafe {
                Some(&*self.num.add(index))
            }
        } else {
            None
        }
    }
    fn display_at_index(&self, index: usize) {
        match self.get_at_index(index) {
            Some(value) => println!("{}", value),
            None => println!("Выход за границы массива."),
        }
    }
    fn delete_at_index(&mut self, index: usize) {
        if index >= self.size {
            return;
        }
        unsafe {
            // Удаляем элемент по индексу
            ptr::drop_in_place(self.num.add(index));
            // Сдвигаем остальные элементы
            if index < self.size - 1 {
                ptr::copy(
                    self.num.add(index + 1),
                    self.num.add(index),
                    self.size - index - 1
                );
            }
        }
        self.size -= 1;
    }
    fn replace_at_index(&mut self, index: usize, value: String) {
        if index >= self.size {
            println!("Нельзя заменить то что не существует.");
            return;
        }
        unsafe {
            *self.num.add(index) = value;
        }
    }
    fn size(&self) -> usize {
        self.size
    }
    fn display_size(&self) {
        println!("{}", self.size);
    }
    fn read_all(&self) {
        if self.size == 0 {
            println!("Массив пустой, Ваша честь.");
            return;
        }
        for i in 0..self.size {
            unsafe {
                print!("{}", &*self.num.add(i));
            }
            if i < self.size - 1 {
                print!(" ");
            }
        }
        println!();
    }
}
impl Drop for Numbers {
    fn drop(&mut self) {
        // Освобождаем все инициализированные элементы
        for i in 0..self.size {
            unsafe {
                ptr::drop_in_place(self.num.add(i));
            }
        }
        // Освобождаем память
        let layout = std::alloc::Layout::array::<String>(self.capacity)
            .expect("Failed to create memory layout");
        unsafe {
            std::alloc::dealloc(self.num as *mut u8, layout);
        }
    }
}
