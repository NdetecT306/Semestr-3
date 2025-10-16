package main

import (
	"fmt"
)

// Numbers - структура массива
type Numbers struct {
	num  []string
	size int
	C    int
}
// createMas - создание массива
func createMas(C int) Numbers {
	return Numbers{
		num:  make([]string, C),
		size: 0,
		C:    C,
	}
}
// addMasAtInd - добавить элемент по индексу
func addMasAtInd(A *Numbers, ind int, znach string) {
	if ind < 0 || ind >= A.C {
		fmt.Println("Индекс вне диапазона емкости массива.")
		return
	}
	if ind >= A.size {
		for i := A.size; i < ind; i++ {
			A.num[i] = ""
		}
		A.size = ind + 1
	}
	A.num[ind] = znach
}
// addMasAtEnd - добавить элемент в конец
func addMasAtEnd(A *Numbers, znach string) {
	if A.size >= A.C {
		fmt.Println("Массив заполнен.")
		return
	}
	A.num[A.size] = znach
	A.size++
}
// poiskMasPoInd - получить элемент по индексу (вывод)
func poiskMasPoInd(A *Numbers, ind int) {
	if ind < 0 || ind >= A.size {
		fmt.Println("Выход за границы массива.")
		return
	}
	fmt.Println(A.num[ind])
}
// deleteMasPoInd - удалить элемент по индексу
func deleteMasPoInd(A *Numbers, ind int) {
	if ind < 0 || ind >= A.size {
		return
	}
	// Сдвигаем элементы влево
	for i := ind; i < A.size-1; i++ {
		A.num[i] = A.num[i+1]
	}
	// Очищаем последний элемент
	A.num[A.size-1] = ""
	A.size-- // Уменьшаем размер
}
// ZamenaMas - заменить элемент по индексу
func ZamenaMas(A *Numbers, ind int, znach string) {
	if ind < 0 || ind >= A.size {
		fmt.Println("Нельзя заменить то что не существует.")
		return
	}
	A.num[ind] = znach
}
// RazmerMas - получить размер массива
func RazmerMas(A *Numbers) {
	fmt.Println(A.size)
}
// readMas - вывести все элементы массива
func readMas(A *Numbers) {
	if A.size == 0 {
		fmt.Println("Массив пустой, Ваша честь.")
		return
	}
	for i := 0; i < A.size; i++ {
		fmt.Print(A.num[i], " ")
	}
	fmt.Println()
}
// getSize - получить размер массива (возвращает значение)
func getSize(A *Numbers) int {
	return A.size
}
// getCapacity - получить емкость массива
func getCapacity(A *Numbers) int {
	return A.C
}
