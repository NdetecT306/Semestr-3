package main

import (
	"errors"
	"fmt"
)

// StudentGroup представляет двусвязный список студентов
type StudentGroup struct {
	head *GroupNode
	tail *GroupNode
}

// GroupNode представляет узел двусвязного списка
type GroupNode struct {
	student string
	next    *GroupNode
	prev    *GroupNode
}

// NewStudentGroup создает новый пустой список студентов
func NewStudentGroup() *StudentGroup {
	return &StudentGroup{}
}

// NewStudentGroupWithFirst создает список с первым студентом
func NewStudentGroupWithFirst(firstStudent string) *StudentGroup {
	node := &GroupNode{student: firstStudent}
	return &StudentGroup{
		head: node,
		tail: node,
	}
}

// findNode находит узел по имени студента
func (sg *StudentGroup) findNode(student string) *GroupNode {
	current := sg.head
	for current != nil {
		if current.student == student {
			return current
		}
		current = current.next
	}
	return nil
}

// AddStudentAfter добавляет студента после указанного
func (sg *StudentGroup) AddStudentAfter(target, newStudent string) {
	targetNode := sg.findNode(target)
	if targetNode == nil {
		return
	}

	newNode := &GroupNode{student: newStudent}
	newNode.next = targetNode.next
	newNode.prev = targetNode

	if targetNode.next != nil {
		targetNode.next.prev = newNode
	}
	targetNode.next = newNode

	if targetNode == sg.tail {
		sg.tail = newNode
	}
}

// AddStudentBefore добавляет студента перед указанным
func (sg *StudentGroup) AddStudentBefore(target, newStudent string) {
	targetNode := sg.findNode(target)
	if targetNode == nil {
		return
	}

	newNode := &GroupNode{student: newStudent}
	newNode.next = targetNode
	newNode.prev = targetNode.prev

	if targetNode.prev != nil {
		targetNode.prev.next = newNode
	} else {
		sg.head = newNode
	}
	targetNode.prev = newNode
}

// DeleteStudentAfter удаляет студента после указанного
func (sg *StudentGroup) DeleteStudentAfter(target string) {
	targetNode := sg.findNode(target)
	if targetNode == nil || targetNode.next == nil {
		return
	}

	nodeToDelete := targetNode.next
	targetNode.next = nodeToDelete.next

	if nodeToDelete.next != nil {
		nodeToDelete.next.prev = targetNode
	}

	if nodeToDelete == sg.tail {
		sg.tail = targetNode
	}
}

// DeleteStudentBefore удаляет студента перед указанным
func (sg *StudentGroup) DeleteStudentBefore(target string) {
	targetNode := sg.findNode(target)
	if targetNode == nil || targetNode.prev == nil {
		return
	}

	nodeToDelete := targetNode.prev

	if nodeToDelete.prev != nil {
		nodeToDelete.prev.next = targetNode
	} else {
		sg.head = targetNode
	}
	targetNode.prev = nodeToDelete.prev

	if nodeToDelete == sg.tail {
		sg.tail = targetNode.prev
	}
}

// AddStudentToStart добавляет студента в начало списка
func (sg *StudentGroup) AddStudentToStart(student string) {
	newNode := &GroupNode{student: student}
	newNode.next = sg.head

	if sg.head != nil {
		sg.head.prev = newNode
	}

	sg.head = newNode

	if sg.tail == nil {
		sg.tail = newNode
	}
}

// AddStudentToEnd добавляет студента в конец списка
func (sg *StudentGroup) AddStudentToEnd(student string) {
	newNode := &GroupNode{student: student}

	if sg.head == nil {
		sg.head = newNode
		sg.tail = newNode
		return
	}

	newNode.prev = sg.tail
	sg.tail.next = newNode
	sg.tail = newNode
}

// DeleteStudentFromStart удаляет студента из начала списка
func (sg *StudentGroup) DeleteStudentFromStart() {
	if sg.head == nil {
		return
	}

	nodeToDelete := sg.head
	sg.head = sg.head.next

	if sg.head != nil {
		sg.head.prev = nil
	}

	if nodeToDelete == sg.tail {
		sg.tail = nil
	}
}

// DeleteStudentFromEnd удаляет студента из конца списка
func (sg *StudentGroup) DeleteStudentFromEnd() {
	if sg.head == nil {
		return
	}

	if sg.head.next == nil {
		sg.head = nil
		sg.tail = nil
		return
	}

	// ИСПРАВЛЕНО: убрано объявление неиспользуемой переменной nodeToDelete
	sg.tail = sg.tail.prev
	sg.tail.next = nil
}

// DeleteStudentByValue удаляет студента по значению
func (sg *StudentGroup) DeleteStudentByValue(student string) error {
	if !sg.Contains(student) {
		return errors.New("нельзя удалить несуществующий элемент")
	}

	nodeToDelete := sg.findNode(student)
	if nodeToDelete == nil {
		return errors.New("элемент не найден")
	}

	if nodeToDelete == sg.head {
		sg.head = nodeToDelete.next
		if sg.head != nil {
			sg.head.prev = nil
		}
	} else {
		if nodeToDelete.prev != nil {
			nodeToDelete.prev.next = nodeToDelete.next
		}
		if nodeToDelete.next != nil {
			nodeToDelete.next.prev = nodeToDelete.prev
		}
	}

	if nodeToDelete == sg.tail {
		sg.tail = nodeToDelete.prev
	}

	return nil
}

// PrintForward выводит список в прямом порядке
func (sg *StudentGroup) PrintForward() {
	current := sg.head
	for current != nil {
		fmt.Print(current.student, " ")
		current = current.next
	}
	fmt.Println()
}

// PrintReverse выводит список в обратном порядке
func (sg *StudentGroup) PrintReverse() {
	current := sg.tail
	for current != nil {
		fmt.Print(current.student, " ")
		current = current.prev
	}
	fmt.Println()
}

// Contains проверяет наличие студента в списке
func (sg *StudentGroup) Contains(student string) bool {
	current := sg.head
	for current != nil {
		if current.student == student {
			return true
		}
		current = current.next
	}
	return false
}

// Clear очищает список
func (sg *StudentGroup) Clear() {
	for sg.head != nil {
		sg.DeleteStudentFromStart()
	}
}

// IsEmpty проверяет, пуст ли список
func (sg *StudentGroup) IsEmpty() bool {
	return sg.head == nil
}

// GetFirst возвращает первого студента в списке
func (sg *StudentGroup) GetFirst() string {
	if sg.head == nil {
		return ""
	}
	return sg.head.student
}

// GetLast возвращает последнего студента в списке
func (sg *StudentGroup) GetLast() string {
	if sg.tail == nil {
		return ""
	}
	return sg.tail.student
}
