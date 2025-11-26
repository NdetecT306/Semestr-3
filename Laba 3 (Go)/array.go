package main

import (
	"errors"
	"fmt"
)

type Array struct {
	num      []string
	size     int
	capacity int
}

func NewArray(c int) *Array {
	return &Array{
		num:      make([]string, c),
		size:     0,
		capacity: c,
	}
}

func (a *Array) Cleanup() {
	// Очищаем массив
	a.size = 0
}

func (a *Array) AddMasAtEnd(znach string) error {
	if a.size >= a.capacity {
		return errors.New("массив заполнен")
	}
	a.num[a.size] = znach
	a.size++
	return nil
}

func (a *Array) ReadMas() {
	if a.size == 0 {
		fmt.Println("Массив пустой, Ваша честь.")
		return
	}
	for i := 0; i < a.size; i++ {
		fmt.Print(a.num[i], " ")
	}
	fmt.Println()
}

func (a *Array) AddMasAtInd(ind int, znach string) error {
	if ind < 0 || ind >= a.capacity {
		return errors.New("индекс вне диапазона емкости массива")
	}
	if ind > a.size {
		return errors.New("индекс превышает текущий размер массива")
	}
	if a.size >= a.capacity {
		return errors.New("массив заполнен")
	}
	
	// Сдвигаем элементы вправо
	for i := a.size; i > ind; i-- {
		a.num[i] = a.num[i-1]
	}
	a.num[ind] = znach
	a.size++
	return nil
}

func (a *Array) PoiskMasPoInd(ind int) (string, error) {
	if ind < 0 || ind >= a.size {
		return "", errors.New("выход за границы массива")
	}
	return a.num[ind], nil
}

func (a *Array) DeleteMasPoInd(ind int) error {
	if ind < 0 || ind >= a.size {
		return errors.New("индекс вне границ массива")
	}
	
	// Сдвигаем элементы влево
	for i := ind; i < a.size-1; i++ {
		a.num[i] = a.num[i+1]
	}
	a.size--
	return nil
}

func (a *Array) ZamenaMas(ind int, znach string) error {
	if ind < 0 || ind >= a.size {
		return errors.New("нельзя заменить то что не существует")
	}
	a.num[ind] = znach
	return nil
}

func (a *Array) RazmerMas() int {
	return a.size
}

func (a *Array) GetCapacity() int {
	return a.capacity
}
