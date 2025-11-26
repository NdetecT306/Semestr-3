package main

import (
	"errors"
	"fmt"
)

// OdSpis представляет узел односвязного списка
type OdSpis struct {
	name  string
	place *OdSpis
}

// NewOdSpis создает новый элемент списка
func NewOdSpis(name string, place *OdSpis) *OdSpis {
	return &OdSpis{
		name:  name,
		place: place,
	}
}

// AddChelPosle добавляет элемент после указанного
func (o *OdSpis) AddChelPosle(ptr *OdSpis, str string) {
	if ptr == nil {
		return
	}
	element := NewOdSpis(str, ptr.place)
	ptr.place = element
}

// AddChelDo добавляет элемент до указанного
func (o *OdSpis) AddChelDo(head **OdSpis, ptr *OdSpis, str string) {
	if ptr == nil {
		return
	}
	
	element := NewOdSpis(str, ptr)
	
	if *head == ptr {
		element.place = *head
		*head = element
		return
	}
	
	current := *head
	for current != nil && current.place != ptr {
		current = current.place
	}
	
	if current != nil {
		current.place = element
	}
}

// AddToEnd добавляет элемент в конец списка
func (o *OdSpis) AddToEnd(ptr **OdSpis, n string) {
	element := NewOdSpis(n, nil)
	
	if *ptr == nil {
		*ptr = element
		return
	}
	
	temp := *ptr
	for temp.place != nil {
		temp = temp.place
	}
	temp.place = element
}

// AddToBegin добавляет элемент в начало списка
func (o *OdSpis) AddToBegin(ptr **OdSpis, n string) {
	element := NewOdSpis(n, *ptr)
	*ptr = element
}

func (node *OdSpis) Print(list *OdSpis) {
    if list == nil {
        fmt.Println()
        return
    }
    temp := list
    for temp != nil {
        fmt.Print(temp.name, " ")
        temp = temp.place
    }
    fmt.Println()
}

// SizeList возвращает размер списка
func (o *OdSpis) SizeList(ptr *OdSpis) int {
	count := 0
	current := ptr
	for current != nil {
		count++
		current = current.place
	}
	return count
}

// Poisk проверяет наличие элемента в списке
func (o *OdSpis) Poisk(ptr *OdSpis, str string) bool {
	current := ptr
	for current != nil {
		if current.name == str {
			return true
		}
		current = current.place
	}
	return false
}

// DeletePoZnach удаляет элемент по значению
func (o *OdSpis) DeletePoZnach(ptr **OdSpis, str string) error {
	if *ptr == nil {
		return errors.New("список пуст")
	}
	
	if !o.Poisk(*ptr, str) {
		return errors.New("нельзя удалить несуществующий элемент")
	}
	
	var prev *OdSpis
	current := *ptr
	
	for current != nil && current.name != str {
		prev = current
		current = current.place
	}
	
	if prev == nil {
		*ptr = current.place
	} else {
		prev.place = current.place
	}
	
	return nil
}

// DeleteToBegin удаляет первый элемент списка
func (o *OdSpis) DeleteToBegin(ptr **OdSpis) {
	if *ptr == nil {
		return
	}
	
	if (*ptr).place == nil {
		*ptr = nil
		return
	}
	
	*ptr = (*ptr).place
}

// DeleteToEnd удаляет последний элемент списка
func (o *OdSpis) DeleteToEnd(ptr **OdSpis) {
	if *ptr == nil {
		return
	}
	
	if (*ptr).place == nil {
		*ptr = nil
		return
	}
	
	temp := *ptr
	for temp.place.place != nil {
		temp = temp.place
	}
	temp.place = nil
}

// FindElement находит элемент и возвращает указатель на него
func (o *OdSpis) FindElement(ptr *OdSpis, str string) *OdSpis {
	current := ptr
	for current != nil {
		if current.name == str {
			return current
		}
		current = current.place
	}
	return nil
}

// DeleteChelDo удаляет элементы до указанного
func (o *OdSpis) DeleteChelDo(ptr **OdSpis, target *OdSpis) {
	if *ptr == nil || target == nil {
		return
	}
	
	if *ptr == target {
		return
	}
	
	var prevPrev *OdSpis
	prev := *ptr
	current := (*ptr).place
	
	for current != nil && current != target {
		prevPrev = prev
		prev = current
		current = current.place
	}
	
	if current != target {
		return
	}
	
	if prevPrev == nil {
		*ptr = current
	} else {
		prevPrev.place = current
	}
}

// DeleteChelPosle удаляет элемент после указанного
func (o *OdSpis) DeleteChelPosle(ptr *OdSpis) {
	if ptr == nil || ptr.place == nil {
		return
	}
	
	ptr.place = ptr.place.place
}

// CleanupList очищает весь список
func (o *OdSpis) CleanupList(list **OdSpis) {
	current := *list
	for current != nil {
		next := current.place
		current = next
	}
	*list = nil
}

