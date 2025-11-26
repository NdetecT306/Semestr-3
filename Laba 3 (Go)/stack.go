package main

import (
	"fmt"
)

type Stack struct {
	head *stackNode
}

type stackNode struct {
	value string
	next  *stackNode
}

func NewStack() *Stack {
	return &Stack{}
}

// Основные методы стека
func (s *Stack) Push(value string) {
	node := &stackNode{
		value: value,
		next:  s.head,
	}
	s.head = node
}

func (s *Stack) IsEmpty() bool {
	return s.head == nil
}

// Методы для совместимости с тестами
func (s *Stack) SPUSH(value string) {
	s.Push(value)
}

func (s *Stack) SPOP() string {
	if s.head == nil {
		return ""
	}
	value := s.head.value
	s.head = s.head.next
	return value
}

func (s *Stack) CountStack() int {
	count := 0
	current := s.head
	for current != nil {
		count++
		current = current.next
	}
	return count
}

func (s *Stack) ReadStack() {
	if s.head == nil {
		fmt.Println("Стек пустой. Читать нечего")
		return
	}
	current := s.head
	for current != nil {
		fmt.Print(current.value, " ")
		current = current.next
	}
	fmt.Println()
}

func (s *Stack) SDELETE() {
	if s.head != nil {
		s.head = s.head.next
	}
}

func (s *Stack) ClearStack() {
	s.head = nil
}
