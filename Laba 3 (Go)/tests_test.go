
package main

import (
	"io"
	"os"
    "testing"
	"fmt"
)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////// Стек
func TestAllStackPrograms(t *testing.T) {
	s := NewStack()
	
	if !s.IsEmpty() {
		t.Error("Стек должен быть пустым")
	}

	s.ReadStack()
	s.SDELETE() // Проверка SDELETE на пустом стеке
	
	if s.CountStack() != 0 {
		t.Errorf("Ожидался размер 0, получен %d", s.CountStack())
	}
	
	if value := s.SPOP(); value != "" {
		t.Errorf("Ожидалась пустая строка, получено '%s'", value)
	}

	s.SPUSH("First") 
	if s.CountStack() != 1 {
		t.Errorf("Ожидался размер 1, получен %d", s.CountStack())
	}
	s.ReadStack()

	s.SPUSH("Second")
	if s.CountStack() != 2 {
		t.Errorf("Ожидался размер 2, получен %d", s.CountStack())
	}
	s.ReadStack()

	s.SPUSH("Third")
	if s.CountStack() != 3 {
		t.Errorf("Ожидался размер 3, получен %d", s.CountStack())
	}
	s.ReadStack()

	// Тестирование SDELETE на стеке с элементами
	s.SDELETE() // Удаляем верхний элемент "Third"
	if s.CountStack() != 2 {
		t.Errorf("После SDELETE ожидался размер 2, получен %d", s.CountStack())
	}
	s.ReadStack()

	// Проверяем, что остался элемент "Second" на вершине
	if value := s.SPOP(); value != "Second" {
		t.Errorf("После SDELETE ожидалось 'Second', получено '%s'", value)
	}
	if s.CountStack() != 1 {
		t.Errorf("Ожидался размер 1, получен %d", s.CountStack())
	}
	s.ReadStack()

	// Еще раз тестируем SDELETE
	s.SPUSH("NewElement")
	s.SPUSH("AnotherElement")
	if s.CountStack() != 3 {
		t.Errorf("Ожидался размер 3, получен %d", s.CountStack())
	}
	s.ReadStack()

	s.SDELETE() // Удаляем "AnotherElement"
	if s.CountStack() != 2 {
		t.Errorf("После SDELETE ожидался размер 2, получен %d", s.CountStack())
	}
	
	// Проверяем, что теперь на вершине "NewElement"
	if value := s.SPOP(); value != "NewElement" {
		t.Errorf("После SDELETE ожидалось 'NewElement', получено '%s'", value)
	}
	if s.CountStack() != 1 {
		t.Errorf("Ожидался размер 1, получен %d", s.CountStack())
	}
	s.ReadStack()

	if value := s.SPOP(); value != "First" {
		t.Errorf("Ожидалось 'First', получено '%s'", value)
	}
	if s.CountStack() != 0 {
		t.Errorf("Ожидался размер 0, получен %d", s.CountStack())
	}
	s.ReadStack()

	// Тестирование последовательных вызовов SDELETE
	s.SPUSH("A")
	s.SPUSH("B")
	s.SPUSH("C")
	if s.CountStack() != 3 {
		t.Errorf("Ожидался размер 3, получен %d", s.CountStack())
	}
	s.ReadStack()

	s.SDELETE() // Удаляем C
	if s.CountStack() != 2 {
		t.Errorf("После SDELETE ожидался размер 2, получен %d", s.CountStack())
	}
	
	s.SDELETE() // Удаляем B
	if s.CountStack() != 1 {
		t.Errorf("После второго SDELETE ожидался размер 1, получен %d", s.CountStack())
	}
	
	s.SDELETE() // Удаляем A
	if s.CountStack() != 0 {
		t.Errorf("После третьего SDELETE ожидался размер 0, получен %d", s.CountStack())
	}
	s.ReadStack()

	// SDELETE на пустом стеке после очистки
	s.SDELETE()
	if s.CountStack() != 0 {
		t.Errorf("SDELETE на пустом стеке не должен менять размер")
	}

	s.SPUSH("El1")
	s.SPUSH("El2")
	s.SPUSH("El3")
	s.SPUSH("El4")
	s.ReadStack()

	if s.CountStack() != 4 {
		t.Errorf("Ожидался размер 4, получен %d", s.CountStack())
	}
	
	// Тестирование SDELETE в середине операций
	s.SDELETE() // Удаляем El4
	if s.CountStack() != 3 {
		t.Errorf("После SDELETE ожидался размер 3, получен %d", s.CountStack())
	}
	
	if value := s.SPOP(); value != "El3" {
		t.Errorf("После SDELETE ожидалось 'El3', получено '%s'", value)
	}
	
	s.ClearStack()
	s.ReadStack()
	if s.CountStack() != 0 {
		t.Errorf("Ожидался размер 0, получен %d", s.CountStack())
	}
	if value := s.SPOP(); value != "" {
		t.Errorf("Ожидалась пустая строка, получено '%s'", value)
	}
	s.ClearStack()
	s.SPUSH("NewElement")
	if s.CountStack() != 1 {
		t.Errorf("Ожидался размер 1, получен %d", s.CountStack())
	}
	s.SDELETE()
	if s.CountStack() != 0 {
		t.Errorf("После SDELETE ожидался размер 0, получен %d", s.CountStack())
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
func TestAllArrayPrograms(t *testing.T) {
	t.Run("BasicOperations", func(t *testing.T) {
		arr := NewArray(5)
		defer arr.Cleanup()
		
		arr.ReadMas() // Должен вывести сообщение о пустом массиве
		if size := arr.RazmerMas(); size != 0 {
			t.Errorf("Ожидался размер 0, получен %d", size)
		}
		if capacity := arr.GetCapacity(); capacity != 5 {
			t.Errorf("Ожидалась емкость 5, получена %d", capacity)
		}
		
		// Тестирование ошибок на пустом массиве
		if err := arr.DeleteMasPoInd(0); err == nil {
			t.Error("Ожидалась ошибка при удалении из пустого массива")
		}
		if _, err := arr.PoiskMasPoInd(0); err == nil {
			t.Error("Ожидалась ошибка при поиске в пустом массиве")
		}

		if err := arr.AddMasAtEnd("First"); err != nil {
			t.Errorf("Ошибка при добавлении: %v", err)
		}
		if size := arr.RazmerMas(); size != 1 {
			t.Errorf("Ожидался размер 1, получен %d", size)
		}
		arr.ReadMas()

		if err := arr.AddMasAtEnd("Second"); err != nil {
			t.Errorf("Ошибка при добавлении: %v", err)
		}
		if size := arr.RazmerMas(); size != 2 {
			t.Errorf("Ожидался размер 2, получен %d", size)
		}
		arr.ReadMas()

		if err := arr.AddMasAtEnd("Third"); err != nil {
			t.Errorf("Ошибка при добавлении: %v", err)
		}
		if size := arr.RazmerMas(); size != 3 {
			t.Errorf("Ожидался размер 3, получен %d", size)
		}
		arr.ReadMas()
	})

	t.Run("InsertAndReplace", func(t *testing.T) {
		arr := NewArray(5)
		defer arr.Cleanup()
		
		arr.AddMasAtEnd("First")
		arr.AddMasAtEnd("Second")
		arr.AddMasAtEnd("Third")
		
		if err := arr.AddMasAtInd(1, "Inserted"); err != nil {
			t.Errorf("Ошибка при вставке: %v", err)
		}
		if size := arr.RazmerMas(); size != 4 {
			t.Errorf("Ожидался размер 4, получен %d", size)
		}
		arr.ReadMas()
		if value, err := arr.PoiskMasPoInd(1); err != nil || value != "Inserted" {
			t.Errorf("Ожидалось 'Inserted' по индексу 1, получено '%s'", value)
		}
		if err := arr.ZamenaMas(0, "Replaced"); err != nil {
			t.Errorf("Ошибка при замене: %v", err)
		}
		if size := arr.RazmerMas(); size != 4 {
			t.Errorf("Ожидался размер 4, получен %d", size)
		}
		arr.ReadMas()
	})
	t.Run("ErrorCases", func(t *testing.T) {
		arr := NewArray(5)
		defer arr.Cleanup()
		arr.AddMasAtEnd("First")
		arr.AddMasAtEnd("Inserted")
		arr.AddMasAtEnd("Second")
		if _, err := arr.PoiskMasPoInd(10); err == nil {
			t.Error("Ожидалась ошибка при поиске по неверному индексу")
		}
		if err := arr.DeleteMasPoInd(-1); err == nil {
			t.Error("Ожидалась ошибка при удалении по отрицательному индексу")
		}
		if err := arr.ZamenaMas(5, "Invalid"); err == nil {
			t.Error("Ожидалась ошибка при замене по неверному индексу")
		}
		if err := arr.DeleteMasPoInd(1); err != nil { // Удаляем "Inserted"
			t.Errorf("Ошибка при удалении: %v", err)
		}
		if size := arr.RazmerMas(); size != 2 {
			t.Errorf("Ожидался размер 2, получен %d", size)
		}
		arr.ReadMas()
	})
	t.Run("CapacityOverflow", func(t *testing.T) {
		arr := NewArray(3)
		defer arr.Cleanup()
		arr.AddMasAtEnd("First")
		arr.AddMasAtEnd("Second")
		if err := arr.AddMasAtEnd("Third"); err != nil {
			t.Errorf("Ошибка при добавлении третьего элемента: %v", err)
		}
		if size := arr.RazmerMas(); size != 3 {
			t.Errorf("Ожидался размер 3, получен %d", size)
		}
		arr.ReadMas()
		if err := arr.AddMasAtEnd("Fourth"); err == nil {
			t.Error("Ожидалась ошибка переполнения")
		}
		if size := arr.RazmerMas(); size != 3 {
			t.Errorf("Размер не должен измениться, ожидался 3, получен %d", size)
		}
	})
	t.Run("DeleteAllAndRestart", func(t *testing.T) {
		arr := NewArray(3)
		defer arr.Cleanup()
		arr.AddMasAtEnd("First")
		arr.AddMasAtEnd("Second")
		arr.AddMasAtEnd("Third")
		if err := arr.DeleteMasPoInd(0); err != nil {
			t.Errorf("Ошибка при удалении: %v", err)
		}
		if size := arr.RazmerMas(); size != 2 {
			t.Errorf("Ожидался размер 2, получен %d", size)
		}
		if err := arr.DeleteMasPoInd(0); err != nil {
			t.Errorf("Ошибка при удаления: %v", err)
		}
		if size := arr.RazmerMas(); size != 1 {
			t.Errorf("Ожидался размер 1, получен %d", size)
		}
		if err := arr.DeleteMasPoInd(0); err != nil {
			t.Errorf("Ошибка при удалении: %v", err)
		}
		if size := arr.RazmerMas(); size != 0 {
			t.Errorf("Ожидался размер 0, получен %d", size)
		}

		if err := arr.AddMasAtEnd("NewStart"); err != nil {
			t.Errorf("Ошибка при добавлении: %v", err)
		}
		if size := arr.RazmerMas(); size != 1 {
			t.Errorf("Ожидался размер 1, получен %d", size)
		}
		arr.ReadMas()
	})
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ОЧЕРЕДЬ
func TestAllQueuePrograms(t *testing.T) {
	queue := NewQueue()
	
	t.Log("Сейчас очередь пуста:")
	queue.ReadQueue()
	
	queue.QDELETE()
	if queue.CountQueue() != 0 {
		t.Errorf("Ожидался размер 0, получен %d", queue.CountQueue())
	}
	
	value, err := queue.QPOP()
	if err == nil || value != "" {
		t.Errorf("Ожидалась ошибка 'очередь пуста', получено '%s'", value)
	}

	queue.QPUSH("First")
	if queue.CountQueue() != 1 {
		t.Errorf("Ожидался размер 1, получен %d", queue.CountQueue())
	}
	t.Log("После добавления First:")
	queue.ReadQueue()

	queue.QPUSH("Second")
	if queue.CountQueue() != 2 {
		t.Errorf("Ожидался размер 2, получен %d", queue.CountQueue())
	}
	t.Log("После добавления Second:")
	queue.ReadQueue()

	queue.QPUSH("Third")
	if queue.CountQueue() != 3 {
		t.Errorf("Ожидался размер 3, получен %d", queue.CountQueue())
	}
	t.Log("После добавления Third:")
	queue.ReadQueue()

	queue.QDELETE()
	if queue.CountQueue() != 2 {
		t.Errorf("Ожидался размер 2, получен %d", queue.CountQueue())
	}
	t.Log("После удаления первого элемента:")
	queue.ReadQueue()

	value, err = queue.QPOP()
	if err != nil {
		t.Errorf("Неожиданная ошибка: %v", err)
	}
	if value != "Second" {
		t.Errorf("Ожидалось 'Second', получено '%s'", value)
	}
	if queue.CountQueue() != 1 {
		t.Errorf("Ожидался размер 1, получен %d", queue.CountQueue())
	}
	t.Log("После извлечения Second:")
	queue.ReadQueue()

	value, err = queue.QPOP()
	if err != nil {
		t.Errorf("Неожиданная ошибка: %v", err)
	}
	if value != "Third" {
		t.Errorf("Ожидалось 'Third', получено '%s'", value)
	}
	if queue.CountQueue() != 0 {
		t.Errorf("Ожидался размер 0, получен %d", queue.CountQueue())
	}
	t.Log("После извлечения Third:")
	queue.ReadQueue()

	queue.QPUSH("A")
	queue.QPUSH("B")
	t.Log("Добавлены A и B:")
	queue.ReadQueue()

	value, err = queue.QPOP()
	if err != nil {
		t.Errorf("Неожиданная ошибка: %v", err)
	}
	if value != "A" {
		t.Errorf("Ожидалось 'A', получено '%s'", value)
	}
	value, err = queue.QPOP()
	if err != nil {
		t.Errorf("Неожиданная ошибка: %v", err)
	}
	if value != "B" {
		t.Errorf("Ожидалось 'B', получено '%s'", value)
	}
	if queue.CountQueue() != 0 {
		t.Errorf("Ожидался размер 0, получен %d", queue.CountQueue())
	}
	t.Log("После извлечения A и B:")
	queue.ReadQueue()

	queue.QPUSH("El1")
	queue.QPUSH("El2")
	queue.QPUSH("El3")
	queue.QPUSH("El4")
	t.Log("Добавлены 4 элемента:")
	queue.ReadQueue()

	if queue.CountQueue() != 4 {
		t.Errorf("Ожидался размер 4, получен %d", queue.CountQueue())
	}
	queue.ClearQueue()
	if queue.CountQueue() != 0 {
		t.Errorf("Ожидался размер 0, получен %d", queue.CountQueue())
	}
	value, err = queue.QPOP()
	if err == nil || value != "" {
		t.Errorf("Ожидалась ошибка 'очередь пуста', получено '%s'", value)
	}
	t.Log("После очистки:")
	queue.ReadQueue()

	queue.QPUSH("NewElement")
	if queue.CountQueue() != 1 {
		t.Errorf("Ожидался размер 1, получен %d", queue.CountQueue())
	}
	value, err = queue.QPOP()
	if err != nil {
		t.Errorf("Неожиданная ошибка: %v", err)
	}
	if value != "NewElement" {
		t.Errorf("Ожидалось 'NewElement', получено '%s'", value)
	}
	if queue.CountQueue() != 0 {
		t.Errorf("Ожидался размер 0, получен %d", queue.CountQueue())
	}
	queue.ClearQueue()
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ОДНОСВЯЗНЫЙ СПИСОК

func TestOdSpisPrint(t *testing.T) {
    // Тест 1: Печать пустого списка
    var emptyList *OdSpis = nil
    emptyList.Print(emptyList) // Не должно быть паники
    
    // Тест 2: Печать списка с одним элементом
    list1 := NewOdSpis("Single", nil)
    defer list1.CleanupList(&list1)
    
    // Захватываем вывод для проверки
    list1.Print(list1)
    
    // Тест 3: Печать списка с несколькими элементами
    list2 := NewOdSpis("First", nil)
    defer list2.CleanupList(&list2)
    list2.AddToEnd(&list2, "Second")
    list2.AddToEnd(&list2, "Third")
    
    list2.Print(list2)
}
func TestOdSpisConstructor(t *testing.T) {
    list := NewOdSpis("First", nil)
    defer list.CleanupList(&list)
    if list.SizeList(list) != 1 {
        t.Errorf("Ожидался размер 1, получен %d", list.SizeList(list))
    }
    if list.name != "First" {
        t.Errorf("Ожидалось 'First', получено '%s'", list.name)
    }
}

func TestOdSpisAddToBegin(t *testing.T) {
    list := NewOdSpis("First", nil)
    defer list.CleanupList(&list)
    list.AddToBegin(&list, "NewFirst")
    if list.SizeList(list) != 2 {
        t.Errorf("Ожидался размер 2, получен %d", list.SizeList(list))
    }
    if !list.Poisk(list, "NewFirst") {
        t.Error("Элемент 'NewFirst' не найден")
    }
    if !list.Poisk(list, "First") {
        t.Error("Элемент 'First' не найден")
    }
    if list.name != "NewFirst" {
        t.Errorf("Ожидалось 'NewFirst', получено '%s'", list.name)
    }
}

func TestOdSpisAddToEnd(t *testing.T) {
    list := NewOdSpis("First", nil)
    defer list.CleanupList(&list)
    list.AddToEnd(&list, "Second")
    list.AddToEnd(&list, "Third")
    if list.SizeList(list) != 3 {
        t.Errorf("Ожидался размер 3, получен %d", list.SizeList(list))
    }
    if !list.Poisk(list, "First") {
        t.Error("Элемент 'First' не найден")
    }
    if !list.Poisk(list, "Second") {
        t.Error("Элемент 'Second' не найден")
    }
    if !list.Poisk(list, "Third") {
        t.Error("Элемент 'Third' не найден")
    }
}

func TestOdSpisAddChelDo(t *testing.T) {
    list := NewOdSpis("Second", nil)
    defer list.CleanupList(&list)
    list.AddToEnd(&list, "Third")
    target := list.FindElement(list, "Second")
    if target == nil {
        t.Fatal("Целевой элемент не найден")
    }
    list.AddChelDo(&list, target, "First")
    if list.SizeList(list) != 3 {
        t.Errorf("Ожидался размер 3, получен %d", list.SizeList(list))
    }
    if !list.Poisk(list, "First") {
        t.Error("Элемент 'First' не найден")
    }
    if list.name != "First" {
        t.Errorf("Ожидалось 'First', получено '%s'", list.name)
    }
}

func TestOdSpisAddChelPosle(t *testing.T) {
    list := NewOdSpis("First", nil)
    defer list.CleanupList(&list)
    list.AddToEnd(&list, "Third")
    target := list.FindElement(list, "First")
    if target == nil {
        t.Fatal("Целевой элемент не найден")
    }
    list.AddChelPosle(target, "Second")
    if list.SizeList(list) != 3 {
        t.Errorf("Ожидался размер 3, получен %d", list.SizeList(list))
    }
    if !list.Poisk(list, "Second") {
        t.Error("Элемент 'Second' не найден")
    }
}

func TestOdSpisAccessMethods(t *testing.T) {
    node2 := NewOdSpis("Second", nil)
    node1 := NewOdSpis("First", node2)
    defer node1.CleanupList(&node1)
    
    if node1.name != "First" {
        t.Errorf("Ожидалось 'First', получено '%s'", node1.name)
    }
    if node1.place != node2 {
        t.Error("Следующий элемент не установлен правильно")
    }
    if node2.name != "Second" {
        t.Errorf("Ожидалось 'Second', получено '%s'", node2.name)
    }
    if node2.place != nil {
        t.Error("Последний элемент должен иметь nil как следующий")
    }
}

func TestOdSpisFindElement(t *testing.T) {
    list := NewOdSpis("First", nil)
    defer list.CleanupList(&list)
    list.AddToEnd(&list, "Second")
    list.AddToEnd(&list, "Third")
    found := list.FindElement(list, "Second")
    if found == nil {
        t.Fatal("Элемент 'Second' не найден")
    }
    if found.name != "Second" {
        t.Errorf("Ожидалось 'Second', получено '%s'", found.name)
    }
    notFound := list.FindElement(list, "NonExistent")
    if notFound != nil {
        t.Error("Несуществующий элемент не должен быть найден")
    }
}

func TestOdSpisDeletePoZnach(t *testing.T) {
    // Тест 1: Удаление из пустого списка
    var emptyList *OdSpis = nil
    err := emptyList.DeletePoZnach(&emptyList, "NonExistent")
    if err == nil {
        t.Error("Ожидалась ошибка при удалении из пустого списка")
    }
    if err.Error() != "список пуст" {
        t.Errorf("Ожидалась ошибка 'список пуст', получено '%s'", err.Error())
    }

    // Тест 2: Удаление несуществующего элемента
    list := NewOdSpis("First", nil)
    defer list.CleanupList(&list)
    list.AddToEnd(&list, "Second")
    
    err = list.DeletePoZnach(&list, "NonExistent")
    if err == nil {
        t.Error("Ожидалась ошибка при удалении несуществующего элемента")
    }
    if err.Error() != "нельзя удалить несуществующий элемент" {
        t.Errorf("Ожидалась ошибка 'нельзя удалить несуществующий элемент', получено '%s'", err.Error())
    }
    if list.SizeList(list) != 2 {
        t.Errorf("Размер списка не должен измениться, ожидался 2, получен %d", list.SizeList(list))
    }

    // Тест 3: Удаление первого элемента
    list2 := NewOdSpis("First", nil)
    defer list2.CleanupList(&list2)
    list2.AddToEnd(&list2, "Second")
    list2.AddToEnd(&list2, "Third")
    
    err = list2.DeletePoZnach(&list2, "First")
    if err != nil {
        t.Errorf("Не ожидалась ошибка при удалении первого элемента: %s", err)
    }
    if list2.SizeList(list2) != 2 {
        t.Errorf("Ожидался размер 2, получен %d", list2.SizeList(list2))
    }
    if list2.Poisk(list2, "First") {
        t.Error("Элемент 'First' не должен быть найден после удаления")
    }
    if list2.name != "Second" {
        t.Errorf("Ожидалось 'Second' как новый первый элемент, получено '%s'", list2.name)
    }
    if !list2.Poisk(list2, "Second") || !list2.Poisk(list2, "Third") {
        t.Error("Элементы 'Second' и 'Third' должны остаться в списке")
    }

    // Тест 4: Удаление среднего элемента
    list3 := NewOdSpis("First", nil)
    defer list3.CleanupList(&list3)
    list3.AddToEnd(&list3, "Second")
    list3.AddToEnd(&list3, "Third")
    
    err = list3.DeletePoZnach(&list3, "Second")
    if err != nil {
        t.Errorf("Не ожидалась ошибка при удалении среднего элемента: %s", err)
    }
    if list3.SizeList(list3) != 2 {
        t.Errorf("Ожидался размер 2, получен %d", list3.SizeList(list3))
    }
    if list3.Poisk(list3, "Second") {
        t.Error("Элемент 'Second' не должен быть найден после удаления")
    }
    if !list3.Poisk(list3, "First") {
        t.Error("Элемент 'First' должен остаться")
    }
    if !list3.Poisk(list3, "Third") {
        t.Error("Элемент 'Third' должен остаться")
    }
    // Проверяем связность
    if list3.place == nil || list3.place.name != "Third" {
        t.Error("Связь между элементами нарушена после удаления среднего элемента")
    }

    // Тест 5: Удаление последнего элемента
    list4 := NewOdSpis("First", nil)
    defer list4.CleanupList(&list4)
    list4.AddToEnd(&list4, "Second")
    list4.AddToEnd(&list4, "Third")
    
    err = list4.DeletePoZnach(&list4, "Third")
    if err != nil {
        t.Errorf("Не ожидалась ошибка при удалении последнего элемента: %s", err)
    }
    if list4.SizeList(list4) != 2 {
        t.Errorf("Ожидался размер 2, получен %d", list4.SizeList(list4))
    }
    if list4.Poisk(list4, "Third") {
        t.Error("Элемент 'Third' не должен быть найден после удаления")
    }
    if !list4.Poisk(list4, "First") || !list4.Poisk(list4, "Second") {
        t.Error("Элементы 'First' и 'Second' должны остаться в списке")
    }
    // Проверяем, что последний элемент теперь "Second"
    temp := list4
    for temp.place != nil {
        temp = temp.place
    }
    if temp.name != "Second" {
        t.Errorf("Ожидалось 'Second' как последний элемент, получено '%s'", temp.name)
    }

    // Тест 6: Удаление единственного элемента
    singleList := NewOdSpis("Single", nil)
    err = singleList.DeletePoZnach(&singleList, "Single")
    if err != nil {
        t.Errorf("Не ожидалась ошибка при удалении единственного элемента: %s", err)
    }
    if singleList != nil {
        t.Error("Список должен быть nil после удаления единственного элемента")
    }
    if singleList != nil && singleList.SizeList(singleList) != 0 {
        t.Errorf("Ожидался размер 0, получен %d", singleList.SizeList(singleList))
    }

    // Тест 7: Проверка печати после удалений
    list5 := NewOdSpis("A", nil)
    defer list5.CleanupList(&list5)
    list5.AddToEnd(&list5, "B")
    list5.AddToEnd(&list5, "C")
    list5.AddToEnd(&list5, "D")
    
    list5.Print(list5) // Печать до удаления
    list5.DeletePoZnach(&list5, "B")
    list5.DeletePoZnach(&list5, "D")
    list5.Print(list5) // Печать после удалений
    
    if list5.SizeList(list5) != 2 {
        t.Errorf("Ожидался размер 2, получен %d", list5.SizeList(list5))
    }
    if !list5.Poisk(list5, "A") || !list5.Poisk(list5, "C") {
        t.Error("Элементы 'A' и 'C' должны остаться в списке")
    }
    if list5.Poisk(list5, "B") || list5.Poisk(list5, "D") {
        t.Error("Элементы 'B' и 'D' не должны быть в списке")
    }
}

func TestOdSpisComplexOperations(t *testing.T) {
    list := NewOdSpis("Start", nil)
    defer list.CleanupList(&list)
    list.AddToEnd(&list, "End")
    target := list.FindElement(list, "End")
    if target == nil {
        t.Fatal("Целевой элемент не найден")
    }
    list.AddChelDo(&list, target, "Middle")
    if list.SizeList(list) != 3 {
        t.Errorf("Ожидался размер 3, получен %d", list.SizeList(list))
    }
    if list.name != "Start" {
        t.Errorf("Ожидалось 'Start', получено '%s'", list.name)
    }
    list.DeleteChelDo(&list, target)
    if list.SizeList(list) != 2 {
        t.Errorf("Ожидался размер 2, получен %d", list.SizeList(list))
    }
    if list.Poisk(list, "Middle") {
        t.Error("Элемент 'Middle' не должен быть найден после удаления")
    }
    start := list.FindElement(list, "Start")
    list.DeleteChelPosle(start)
    if list.SizeList(list) != 1 {
        t.Errorf("Ожидался размер 1, получен %d", list.SizeList(list))
    }
    if list.Poisk(list, "End") {
        t.Error("Элемент 'End' не должен быть найден после удаления")
    }
    if list.name != "Start" {
        t.Errorf("Ожидалось 'Start', получено '%s'", list.name)
    }
}

func TestOdSpisDeleteToBegin(t *testing.T) {
    list := NewOdSpis("First", nil)
    defer list.CleanupList(&list)
    list.AddToEnd(&list, "Second")
    list.AddToEnd(&list, "Third")
    
    if list.SizeList(list) != 3 {
        t.Errorf("Ожидался размер 3, получен %d", list.SizeList(list))
    }
    
    // Удаляем первый элемент
    list.DeleteToBegin(&list)
    if list.SizeList(list) != 2 {
        t.Errorf("Ожидался размер 2, получен %d", list.SizeList(list))
    }
    if list.name != "Second" {
        t.Errorf("Ожидалось 'Second', получено '%s'", list.name)
    }
    if list.Poisk(list, "First") {
        t.Error("Элемент 'First' не должен быть найден после удаления")
    }
    
    // Удаляем еще один элемент
    list.DeleteToBegin(&list)
    if list.SizeList(list) != 1 {
        t.Errorf("Ожидался размер 1, получен %d", list.SizeList(list))
    }
    if list.name != "Third" {
        t.Errorf("Ожидалось 'Third', получено '%s'", list.name)
    }
    
    // Удаляем последний элемент
    list.DeleteToBegin(&list)
    if list.SizeList(list) != 0 {
        t.Errorf("Ожидался размер 0, получен %d", list.SizeList(list))
    }
    if list != nil {
        t.Error("Список должен быть nil после удаления всех элементов")
    }
}

func TestOdSpisDeleteToEnd(t *testing.T) {
    list := NewOdSpis("First", nil)
    defer list.CleanupList(&list)
    list.AddToEnd(&list, "Second")
    list.AddToEnd(&list, "Third")
    
    if list.SizeList(list) != 3 {
        t.Errorf("Ожидался размер 3, получен %d", list.SizeList(list))
    }
    
    // Удаляем последний элемент
    list.DeleteToEnd(&list)
    if list.SizeList(list) != 2 {
        t.Errorf("Ожидался размер 2, получен %d", list.SizeList(list))
    }
    if list.Poisk(list, "Third") {
        t.Error("Элемент 'Third' не должен быть найден после удаления")
    }
    
    // Проверяем, что последний элемент теперь "Second"
    temp := list
    for temp.place != nil {
        temp = temp.place
    }
    if temp.name != "Second" {
        t.Errorf("Ожидалось 'Second', получено '%s'", temp.name)
    }
    
    // Удаляем еще один элемент с конца
    list.DeleteToEnd(&list)
    if list.SizeList(list) != 1 {
        t.Errorf("Ожидался размер 1, получен %d", list.SizeList(list))
    }
    if list.Poisk(list, "Second") {
        t.Error("Элемент 'Second' не должен быть найден после удаления")
    }
    if list.name != "First" {
        t.Errorf("Ожидалось 'First', получено '%s'", list.name)
    }
    
    // Удаляем последний оставшийся элемент
    list.DeleteToEnd(&list)
    if list.SizeList(list) != 0 {
        t.Errorf("Ожидался размер 0, получен %d", list.SizeList(list))
    }
    if list != nil {
        t.Error("Список должен быть nil после удаления всех элементов")
    }
}

func TestOdSpisDeleteToBeginEmptyList(t *testing.T) {
    var list *OdSpis = nil
    // Не должно быть паники при удалении из пустого списка
    list.DeleteToBegin(&list)
    if list != nil {
        t.Error("Пустой список должен остаться nil")
    }
}

func TestOdSpisDeleteToEndEmptyList(t *testing.T) {
    var list *OdSpis = nil
    // Не должно быть паники при удалении из пустого списка
    list.DeleteToEnd(&list)
    if list != nil {
        t.Error("Пустой список должен остаться nil")
    }
}

func TestOdSpisDeleteToBeginSingleElement(t *testing.T) {
    list := NewOdSpis("Single", nil)
    defer list.CleanupList(&list)
    
    list.DeleteToBegin(&list)
    if list != nil {
        t.Error("Список должен быть nil после удаления единственного элемента")
    }
}

func TestOdSpisDeleteToEndSingleElement(t *testing.T) {
    list := NewOdSpis("Single", nil)
    defer list.CleanupList(&list)
    
    list.DeleteToEnd(&list)
    if list != nil {
        t.Error("Список должен быть nil после удаления единственного элемента")
    }
}

func TestOdSpisMixedDeletions(t *testing.T) {
    list := NewOdSpis("A", nil)
    defer list.CleanupList(&list)
    list.AddToEnd(&list, "B")
    list.AddToEnd(&list, "C")
    list.AddToEnd(&list, "D")
    list.AddToEnd(&list, "E")
    
    if list.SizeList(list) != 5 {
        t.Errorf("Ожидался размер 5, получен %d", list.SizeList(list))
    }
    
    // Удаляем с начала
    list.DeleteToBegin(&list)
    if list.name != "B" {
        t.Errorf("Ожидалось 'B', получено '%s'", list.name)
    }
    
    // Удаляем с конца
    list.DeleteToEnd(&list)
    if list.SizeList(list) != 3 {
        t.Errorf("Ожидался размер 3, получен %d", list.SizeList(list))
    }
    if list.Poisk(list, "E") {
        t.Error("Элемент 'E' не должен быть найден после удаления")
    }
    
    // Еще раз с начала
    list.DeleteToBegin(&list)
    if list.name != "C" {
        t.Errorf("Ожидалось 'C', получено '%s'", list.name)
    }
    
    // Проверяем оставшиеся элементы
    if !list.Poisk(list, "C") || !list.Poisk(list, "D") {
        t.Error("Элементы 'C' и 'D' должны остаться в списке")
    }
    if list.SizeList(list) != 2 {
        t.Errorf("Ожидался размер 2, получен %d", list.SizeList(list))
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ХЭШ
func TestDoubleHashTableAddAndRetrieve(t *testing.T) {
	table := NewDoubleHashTable()
	
	// Проверка начального размера и количества элементов
	if table.Size() != HASH_SIZE {
		t.Errorf("Неверный размер таблицы. Ожидалось %d, получено %d", HASH_SIZE, table.Size())
	}
	if table.ElementCount() != 0 {
		t.Errorf("Начальное количество элементов должно быть 0, получено %d", table.ElementCount())
	}
	
	if !table.ADD(1, "value1") {
		t.Error("Не удалось добавить элемент с ключом 1")
	}
	if !table.ADD(2, "value2") {
		t.Error("Не удалось добавить элемент с ключом 2")
	}
	if !table.ADD(3, "value3") {
		t.Error("Не удалось добавить элемент с ключом 3")
	}
	
	// Проверка количества элементов после добавления
	if table.ElementCount() != 3 {
		t.Errorf("Ожидалось 3 элемента, получено %d", table.ElementCount())
	}
	
	// Проверка через SET_AT (поиск по ключу и значению)
	if !table.SET_AT(1, "value1") {
		t.Error("Элемент с ключом 1 и значением 'value1' не найден")
	}
	if !table.SET_AT(2, "value2") {
		t.Error("Элемент с ключом 2 и значением 'value2' не найден")
	}
	if !table.SET_AT(3, "value3") {
		t.Error("Элемент с ключом 3 и значением 'value3' не найден")
	}
	
	// Проверка ContainsKey
	if !table.ContainsKey(1) {
		t.Error("ContainsKey(1) должен возвращать true")
	}
	if !table.ContainsKey(2) {
		t.Error("ContainsKey(2) должен возвращать true")
	}
	if !table.ContainsKey(3) {
		t.Error("ContainsKey(3) должен возвращать true")
	}
	
	// SET_AT должен возвращать false для неверных значений
	if table.SET_AT(1, "wrong_value") {
		t.Error("SET_AT(1, 'wrong_value') должен возвращать false для неверного значения")
	}
	if table.SET_AT(999, "nonexistent") {
		t.Error("SET_AT(999, 'nonexistent') должен возвращать false для несуществующего ключа")
	}
	
	// Проверка ContainsKey для несуществующего ключа
	if table.ContainsKey(999) {
		t.Error("ContainsKey(999) должен возвращать false")
	}
}

func TestDoubleHashTableAddDuplicate(t *testing.T) {
	table := NewDoubleHashTable()
	
	if !table.ADD(1, "value1") {
		t.Error("Не удалось добавить элемент с ключом 1")
	}
	
	// Проверка ContainsKey после добавления
	if !table.ContainsKey(1) {
		t.Error("ContainsKey(1) должен возвращать true после добавления")
	}
	
	// Попытка добавить дубликат с тем же ключом и значением
	if table.ADD(1, "value1") {
		t.Error("Дубликат (1, 'value1') не должен быть добавлен")
	}
	
	// Попытка добавить элемент с тем же ключом но другим значением
	if table.ADD(1, "value2") {
		t.Error("Элемент с существующим ключом 1 не должен быть добавлен")
	}
	
	// Количество элементов не должно измениться
	if table.ElementCount() != 1 {
		t.Errorf("Количество элементов должно остаться 1, получено %d", table.ElementCount())
	}
}

func TestDoubleHashTableRemove(t *testing.T) {
	table := NewDoubleHashTable()
	
	if !table.ADD(1, "value1") {
		t.Error("Не удалось добавить элемент с ключом 1")
	}
	
	// Проверка перед удалением
	if !table.ContainsKey(1) {
		t.Error("ContainsKey(1) должен возвращать true перед удалением")
	}
	if table.ElementCount() != 1 {
		t.Errorf("Ожидалось 1 элемент перед удалением, получено %d", table.ElementCount())
	}
	
	if !table.SETDEL(1, "value1") {
		t.Error("Не удалось удалить элемент (1, 'value1')")
	}
	
	// Проверка после удаления
	if table.SET_AT(1, "value1") {
		t.Error("Элемент (1, 'value1') не должен быть найден после удаления")
	}
	if table.ContainsKey(1) {
		t.Error("ContainsKey(1) должен возвращать false после удаления")
	}
	if table.ElementCount() != 0 {
		t.Errorf("Ожидалось 0 элементов после удаления, получено %d", table.ElementCount())
	}
	
	// Добавляем и удаляем другой элемент
	table.ADD(2, "value2")
	if !table.SETDEL(2, "value2") {
		t.Error("Не удалось удалить элемент (2, 'value2')")
	}
	if table.SET_AT(2, "value2") {
		t.Error("Элемент (2, 'value2') не должен существовать после удаления")
	}
	if table.ContainsKey(2) {
		t.Error("ContainsKey(2) должен возвращать false после удаления")
	}
	
	// Удаление несуществующего элемента
	if table.SETDEL(1, "value1") {
		t.Error("Удаление несуществующего элемента должно возвращать false")
	}
}

func TestDoubleHashTableComplexScenario(t *testing.T) {
	table := NewDoubleHashTable()
	
	table.ADD(1, "one")
	table.ADD(2, "two")
	table.ADD(3, "three")
	
	// Проверка начального состояния
	if table.ElementCount() != 3 {
		t.Errorf("Ожидалось 3 элемента, получено %d", table.ElementCount())
	}
	if !table.ContainsKey(1) || !table.ContainsKey(2) || !table.ContainsKey(3) {
		t.Error("Все ключи 1, 2, 3 должны существовать")
	}
	
	// Удаляем элемент
	table.SETDEL(2, "two")
	
	// Проверка после удаления
	if table.ElementCount() != 2 {
		t.Errorf("Ожидалось 2 элемента после удаления, получено %d", table.ElementCount())
	}
	if !table.ContainsKey(1) {
		t.Error("Элемент с ключом 1 должен существовать")
	}
	if table.ContainsKey(2) {
		t.Error("Элемент с ключом 2 не должен существовать после удаления")
	}
	if !table.ContainsKey(3) {
		t.Error("Элемент с ключом 3 должен существовать")
	}
	
	// Добавляем новый элемент
	if !table.ADD(4, "four") {
		t.Error("Не удалось добавить элемент (4, 'four')")
	}
	
	// Проверка через GetAllElements и ElementCount
	_, _, count := table.GetAllElements()
	if count != 3 { // 1, 3, 4
		t.Errorf("GetAllElements: ожидалось 3 элемента, получено %d", count)
	}
	if table.ElementCount() != 3 {
		t.Errorf("ElementCount: ожидалось 3 элемента, получено %d", table.ElementCount())
	}
	
	// Добавляем элемент с ключом 2 (ранее удаленным)
	if !table.ADD(2, "new_two") {
		t.Error("Не удалось добавить элемент (2, 'new_two')")
	}
	
	// Финальная проверка
	_, _, count = table.GetAllElements()
	if count != 4 { // 1, 2, 3, 4
		t.Errorf("Ожидалось 4 элемента, получено %d", count)
	}
	if table.ElementCount() != 4 {
		t.Errorf("ElementCount: ожидалось 4 элемента, получено %d", table.ElementCount())
	}
	if !table.ContainsKey(2) {
		t.Error("ContainsKey(2) должен возвращать true после повторного добавления")
	}
}

func TestDoubleHashTableReuseDeletedSlots(t *testing.T) {
	table := NewDoubleHashTable()
	
	// Добавляем элементы
	if !table.ADD(1, "value1") {
		t.Error("Не удалось добавить элемент с ключом 1")
	}
	if !table.ADD(2, "value2") {
		t.Error("Не удалось добавить элемент с ключом 2")
	}
	
	// Удаляем элемент
	if !table.SETDEL(1, "value1") {
		t.Error("Не удалось удалить элемент с ключом 1")
	}
	
	// Добавляем новый элемент - должен использовать освободившийся слот
	if !table.ADD(3, "value3") {
		t.Error("Не удалось добавить элемент с ключом 3 после удаления")
	}
	
	// Проверяем, что новые элементы доступны
	if !table.SET_AT(2, "value2") {
		t.Error("Элемент с ключом 2 должен быть доступен")
	}
	if !table.SET_AT(3, "value3") {
		t.Error("Элемент с ключом 3 должен быть доступен")
	}
	
	// Проверяем, что старый элемент недоступен
	if table.SET_AT(1, "value1") {
		t.Error("Удаленный элемент с ключом 1 не должен быть доступен")
	}
}

func TestDoubleHashTableSizeConsistency(t *testing.T) {
	table := NewDoubleHashTable()
	table.INIT_HASH_TABLE()
	
	// Размер таблицы должен быть постоянным
	if table.Size() != HASH_SIZE {
		t.Errorf("Размер таблицы должен быть %d, получено %d", HASH_SIZE, table.Size())
	}
	
	// Добавляем и удаляем элементы
	for i := 0; i < 10; i++ {
		table.ADD(i, fmt.Sprintf("value%d", i))
	}
	
	// Размер не должен измениться
	if table.Size() != HASH_SIZE {
		t.Errorf("Размер таблицы должен остаться %d после добавления элементов, получено %d", HASH_SIZE, table.Size())
	}
	
	// Удаляем некоторые элементы
	for i := 0; i < 5; i++ {
		table.SETDEL(i, fmt.Sprintf("value%d", i))
	}
	
	// Размер все еще не должен измениться
	if table.Size() != HASH_SIZE {
		t.Errorf("Размер таблицы должен остаться %d после удаления элементов, получено %d", HASH_SIZE, table.Size())
	}
}

func TestDoubleHashTableEmptyTable(t *testing.T) {
	table := NewDoubleHashTable()
	table.INIT_HASH_TABLE()
	
	// Проверка пустой таблицы
	if table.ElementCount() != 0 {
		t.Errorf("Пустая таблица должна содержать 0 элементов, получено %d", table.ElementCount())
	}
	if table.Size() != HASH_SIZE {
		t.Errorf("Размер пустой таблицы должен быть %d, получено %d", HASH_SIZE, table.Size())
	}
	
	// ContainsKey для несуществующих ключей
	if table.ContainsKey(1) {
		t.Error("ContainsKey(1) должен возвращать false для пустой таблицы")
	}
	if table.ContainsKey(999) {
		t.Error("ContainsKey(999) должен возвращать false для пустой таблицы")
	}
	
	// SET_AT для несуществующих ключей
	if table.SET_AT(1, "any_value") {
		t.Error("SET_AT(1) должен возвращать false для пустой таблицы")
	}
}

func TestDoubleHashTableCollisions(t *testing.T) {
	table := NewDoubleHashTable()
	table.INIT_HASH_TABLE()
	
	// Создаем ключи, которые могут вызывать коллизии
	// Добавляем несколько элементов
	keys := []int{1, 2, 3, 4, 5}
	for i, key := range keys {
		if !table.ADD(key, fmt.Sprintf("value%d", i)) {
			t.Errorf("Не удалось добавить элемент с ключом %d", key)
		}
	}
	
	// Проверяем, что все элементы доступны
	for i, key := range keys {
		if !table.SET_AT(key, fmt.Sprintf("value%d", i)) {
			t.Errorf("Элемент с ключом %d не найден после добавления", key)
		}
	}
	
	// Проверяем количество элементов
	if table.ElementCount() != len(keys) {
		t.Errorf("Ожидалось %d элементов, получено %d", len(keys), table.ElementCount())
	}
}

func TestDoubleHashTableFullTable(t *testing.T) {
	table := NewDoubleHashTable()
	table.INIT_HASH_TABLE()
	
	// Заполняем таблицу до предела
	addedCount := 0
	for i := 0; i < HASH_SIZE*2; i++ { // Пытаемся добавить больше элементов, чем вмещает таблица
		if table.ADD(i, fmt.Sprintf("value%d", i)) {
			addedCount++
		}
		if addedCount >= HASH_SIZE {
			break // Таблица заполнена
		}
	}
	
	// После заполнения таблицы добавление должно возвращать false
	if table.ADD(HASH_SIZE+100, "should_fail") {
		t.Error("Добавление в заполненную таблицу должно возвращать false")
	}
	
	// Проверяем, что количество элементов равно размеру таблицы
	if table.ElementCount() != HASH_SIZE {
		t.Errorf("В заполненной таблице должно быть %d элементов, получено %d", HASH_SIZE, table.ElementCount())
	}
}


func TestDoubleHashTableWrapAround(t *testing.T) {
	table := NewDoubleHashTable()
	
	// Используем ключи, которые создают разные начальные индексы но могут вызывать обход
	successfulAdds := 0
	for i := 0; i < HASH_SIZE; i++ {
		// Создаем ключи, которые гарантированно разные для hash1
		key := i*13 + 1 // Простое число для разнообразия хешей
		if table.ADD(key, fmt.Sprintf("value%d", i)) {
			successfulAdds++
		}
	}
	
	t.Logf("Успешно добавлено %d элементов из %d попыток", successfulAdds, HASH_SIZE)
	
	// Проверяем, что все добавленные элементы доступны
	for i := 0; i < successfulAdds; i++ {
		key := i*13 + 1
		if !table.SET_AT(key, fmt.Sprintf("value%d", i)) {
			t.Errorf("Элемент с ключом %d не найден после обхода таблицы", key)
		}
	}
	
	// Проверяем, что таблица действительно заполнена
	if successfulAdds < HASH_SIZE {
		t.Logf("Таблица заполнена не полностью: %d/%d элементов", successfulAdds, HASH_SIZE)
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ДЕРЕВО
// TestFixDelete проверяет балансировку после удаления через публичный интерфейс
func TestFixDelete(t *testing.T) {
	t.Run("DeleteRedLeafNoFixNeeded", func(t *testing.T) {
		root := CreateRoot(20)
		root = AddLeaf(root, 10)
		root = AddLeaf(root, 30)
		root = AddLeaf(root, 5)  // Красный лист
		root = AddLeaf(root, 15) // Красный лист

		// Удаляем красный лист - балансировка не требуется
		root = DeleteNode(root, 5)
		checkRedBlackProperties(t, root)

		// Удаляем другой красный лист
		root = DeleteNode(root, 15)
		checkRedBlackProperties(t, root)

		// Очистка
		root = DeleteNode(root, 10)
		root = DeleteNode(root, 20)
		root = DeleteNode(root, 30)
	})

	t.Run("DeleteBlackNodeWithRedChild", func(t *testing.T) {
		root := CreateRoot(20)
		root = AddLeaf(root, 10)
		root = AddLeaf(root, 30)
		root = AddLeaf(root, 5)  // Черный узел с красным ребенком
		root = AddLeaf(root, 15) // Черный узел

		// Удаляем черный узел с красным ребенком
		root = DeleteNode(root, 10)
		checkRedBlackProperties(t, root)

		// Очистка
		root = DeleteNode(root, 5)
		root = DeleteNode(root, 15)
		root = DeleteNode(root, 20)
		root = DeleteNode(root, 30)
	})

	t.Run("DeleteBlackLeafCase1", func(t *testing.T) {
		// Создаем структуру, где удаление черного листа вызовет случай 1
		root := CreateRoot(50)
		root = AddLeaf(root, 30)
		root = AddLeaf(root, 70)
		root = AddLeaf(root, 20)
		root = AddLeaf(root, 40)
		root = AddLeaf(root, 60)
		root = AddLeaf(root, 80)

		// Удаляем черный лист, который может вызвать случай 1 (красный брат)
		root = DeleteNode(root, 20)
		checkRedBlackProperties(t, root)

		// Очистка
		root = DeleteNode(root, 30)
		root = DeleteNode(root, 40)
		root = DeleteNode(root, 50)
		root = DeleteNode(root, 60)
		root = DeleteNode(root, 70)
		root = DeleteNode(root, 80)
	})

	t.Run("DeleteBlackLeafCase2", func(t *testing.T) {
		// Создаем структуру для случая 2 (оба ребенка брата черные)
		root := CreateRoot(50)
		root = AddLeaf(root, 30)
		root = AddLeaf(root, 70)
		root = AddLeaf(root, 20)
		root = AddLeaf(root, 40)
		root = AddLeaf(root, 10) // Добавляем для создания нужной структуры

		// Удаляем узел, который может вызвать случай 2
		root = DeleteNode(root, 40)
		checkRedBlackProperties(t, root)

		// Очистка
		root = DeleteNode(root, 10)
		root = DeleteNode(root, 20)
		root = DeleteNode(root, 30)
		root = DeleteNode(root, 50)
		root = DeleteNode(root, 70)
	})

	t.Run("DeleteBlackLeafCase3", func(t *testing.T) {
		// Создаем структуру для случая 3 (правый ребенок брата черный)
		root := CreateRoot(50)
		root = AddLeaf(root, 30)
		root = AddLeaf(root, 70)
		root = AddLeaf(root, 20)
		root = AddLeaf(root, 40)
		root = AddLeaf(root, 35) // Создаем левого красного ребенка у брата

		// Удаляем узел, который может вызвать случай 3
		root = DeleteNode(root, 20)
		checkRedBlackProperties(t, root)

		// Очистка
		root = DeleteNode(root, 30)
		root = DeleteNode(root, 35)
		root = DeleteNode(root, 40)
		root = DeleteNode(root, 50)
		root = DeleteNode(root, 70)
	})

	t.Run("DeleteBlackLeafCase4", func(t *testing.T) {
		// Создаем структуру для случая 4 (правый ребенок брата красный)
		root := CreateRoot(50)
		root = AddLeaf(root, 30)
		root = AddLeaf(root, 70)
		root = AddLeaf(root, 20)
		root = AddLeaf(root, 40)
		root = AddLeaf(root, 45) // Создаем правого красного ребенка у брата

		// Удаляем узел, который может вызвать случай 4
		root = DeleteNode(root, 20)
		checkRedBlackProperties(t, root)

		// Очистка
		root = DeleteNode(root, 30)
		root = DeleteNode(root, 40)
		root = DeleteNode(root, 45)
		root = DeleteNode(root, 50)
		root = DeleteNode(root, 70)
	})

	t.Run("DeleteRootWithChildren", func(t *testing.T) {
		root := CreateRoot(50)
		root = AddLeaf(root, 30)
		root = AddLeaf(root, 70)

		// Удаляем корень с двумя детьми
		root = DeleteNode(root, 50)
		checkRedBlackProperties(t, root)

		if root == nil {
			t.Error("После удаления корня дерево не должно быть пустым")
		}

		// Очистка
		if root != nil {
			root = DeleteNode(root, 30)
			root = DeleteNode(root, 70)
		}
	})

	t.Run("DeleteComplexScenario", func(t *testing.T) {
		root := CreateRoot(100)
		values := []int{50, 150, 25, 75, 125, 175, 12, 37, 63, 87, 112, 137, 162, 187}
		
		// Вставляем все значения
		for _, val := range values {
			root = AddLeaf(root, val)
		}
		checkRedBlackProperties(t, root)

		// Удаляем в случайном порядке, проверяя свойства после каждого удаления
		deleteOrder := []int{25, 125, 75, 150, 37, 112, 187, 50}
		for _, val := range deleteOrder {
			root = DeleteNode(root, val)
			checkRedBlackProperties(t, root)
		}

		// Удаляем оставшиеся узлы
		remaining := []int{12, 63, 87, 100, 137, 162, 175}
		for _, val := range remaining {
			root = DeleteNode(root, val)
			if root != nil {
				checkRedBlackProperties(t, root)
			}
		}
	})
}

// TestFixDeleteEdgeCases проверяет крайние случаи балансировки после удаления
func TestFixDeleteEdgeCases(t *testing.T) {
	t.Run("DeleteFromEmptyTree", func(t *testing.T) {
		var root *Tree
		root = DeleteNode(root, 10)
		if root != nil {
			t.Error("Удаление из пустого дерева должно возвращать nil")
		}
	})

	t.Run("DeleteNonExistentNode", func(t *testing.T) {
		root := CreateRoot(10)
		root = AddLeaf(root, 5)
		root = AddLeaf(root, 15)

		originalRoot := root
		root = DeleteNode(root, 999) // Несуществующий узел

		if root != originalRoot {
			t.Error("При удалении несуществующего узла корень не должен меняться")
		}
		checkRedBlackProperties(t, root)

		// Очистка
		root = DeleteNode(root, 5)
		root = DeleteNode(root, 10)
		root = DeleteNode(root, 15)
	})

	t.Run("DeleteSingleNodeTree", func(t *testing.T) {
		root := CreateRoot(42)
		root = DeleteNode(root, 42)
		
		if root != nil {
			t.Error("После удаления единственного узла дерево должно быть пустым")
		}
	})

	t.Run("DeleteAndMaintainBlackHeight", func(t *testing.T) {
		// Проверяем, что после удалений сохраняется черная высота
		root := CreateRoot(50)
		root = AddLeaf(root, 25)
		root = AddLeaf(root, 75)
		root = AddLeaf(root, 12)
		root = AddLeaf(root, 37)
		root = AddLeaf(root, 62)
		root = AddLeaf(root, 87)

		// Измеряем черную высоту до удаления
		tree := &Tree{}
		initialHeight := tree.GetHeight(root)

		// Удаляем несколько узлов
		root = DeleteNode(root, 12)
		root = DeleteNode(root, 37)
		checkRedBlackProperties(t, root)

		// Черная высота может уменьшиться, но свойства должны сохраниться
		finalHeight := tree.GetHeight(root)
		if finalHeight > initialHeight+2 {
			t.Errorf("Черная высота не должна значительно увеличиваться после удаления. Было: %d, стало: %d", 
				initialHeight, finalHeight)
		}

		// Очистка
		root = DeleteNode(root, 25)
		root = DeleteNode(root, 50)
		root = DeleteNode(root, 62)
		root = DeleteNode(root, 75)
		root = DeleteNode(root, 87)
	})

	t.Run("DeleteAllNodesSequentially", func(t *testing.T) {
		root := CreateRoot(100)
		values := []int{50, 150, 25, 75, 125, 175}
		
		for _, val := range values {
			root = AddLeaf(root, val)
		}
		checkRedBlackProperties(t, root)

		// Удаляем все узлы по одному
		allValues := append([]int{100}, values...)
		for _, val := range allValues {
			root = DeleteNode(root, val)
			if root != nil {
				checkRedBlackProperties(t, root)
			}
		}

		if root != nil {
			t.Error("После удаления всех узлов дерево должно быть пустым")
		}
	})
}

// TestFixDeleteSpecificStructures проверяет удаление из специфических структур дерева
func TestFixDeleteSpecificStructures(t *testing.T) {
	t.Run("DeleteFromLeftHeavyTree", func(t *testing.T) {
		root := CreateRoot(50)
		// Создаем лево-стороннее дерево
		root = AddLeaf(root, 30)
		root = AddLeaf(root, 20)
		root = AddLeaf(root, 10)
		root = AddLeaf(root, 5)
		root = AddLeaf(root, 40)

		checkRedBlackProperties(t, root)

		// Удаляем узлы с левой стороны
		root = DeleteNode(root, 5)
		checkRedBlackProperties(t, root)

		root = DeleteNode(root, 10)
		checkRedBlackProperties(t, root)

		root = DeleteNode(root, 20)
		checkRedBlackProperties(t, root)

		// Очистка
		root = DeleteNode(root, 30)
		root = DeleteNode(root, 40)
		root = DeleteNode(root, 50)
	})

	t.Run("DeleteFromRightHeavyTree", func(t *testing.T) {
		root := CreateRoot(50)
		// Создаем право-стороннее дерево
		root = AddLeaf(root, 70)
		root = AddLeaf(root, 80)
		root = AddLeaf(root, 90)
		root = AddLeaf(root, 85)
		root = AddLeaf(root, 60)

		checkRedBlackProperties(t, root)

		// Удаляем узлы с правой стороны
		root = DeleteNode(root, 85)
		checkRedBlackProperties(t, root)

		root = DeleteNode(root, 90)
		checkRedBlackProperties(t, root)

		root = DeleteNode(root, 80)
		checkRedBlackProperties(t, root)

		// Очистка
		root = DeleteNode(root, 50)
		root = DeleteNode(root, 60)
		root = DeleteNode(root, 70)
	})

	t.Run("DeleteFromBalancedTree", func(t *testing.T) {
		root := CreateRoot(50)
		// Создаем сбалансированное дерево
		root = AddLeaf(root, 25)
		root = AddLeaf(root, 75)
		root = AddLeaf(root, 12)
		root = AddLeaf(root, 37)
		root = AddLeaf(root, 62)
		root = AddLeaf(root, 87)
		root = AddLeaf(root, 6)
		root = AddLeaf(root, 18)
		root = AddLeaf(root, 31)
		root = AddLeaf(root, 43)

		checkRedBlackProperties(t, root)

		// Удаляем узлы в разном порядке, сохраняя баланс
		deleteOrder := []int{6, 87, 31, 25, 62}
		for _, val := range deleteOrder {
			root = DeleteNode(root, val)
			checkRedBlackProperties(t, root)
		}

		// Очистка оставшихся узлов
		remaining := []int{12, 18, 37, 43, 50, 75}
		for _, val := range remaining {
			root = DeleteNode(root, val)
			if root != nil {
				checkRedBlackProperties(t, root)
			}
		}
	})
}

// TestFixDeletePerformance проверяет производительность при множественных удалениях
func TestFixDeletePerformance(t *testing.T) {
	t.Run("MassInsertAndDelete", func(t *testing.T) {
		root := CreateRoot(500)
		
		// Вставляем 100 узлов
		for i := 1; i <= 100; i++ {
			if i != 500 { // избегаем дублирования корня
				root = AddLeaf(root, i)
			}
		}
		checkRedBlackProperties(t, root)

		// Удаляем 50 узлов
		for i := 1; i <= 50; i++ {
			root = DeleteNode(root, i)
			checkRedBlackProperties(t, root)
		}

		// Проверяем, что оставшиеся узлы доступны
		for i := 51; i <= 100; i++ {
			if !IfValueExists(root, i) {
				t.Errorf("Узел %d должен существовать после удалений", i)
			}
		}

		// Очистка
		for i := 51; i <= 100; i++ {
			root = DeleteNode(root, i)
		}
		root = DeleteNode(root, 500)
	})
}
// TestCreateNode проверяет создание нового узла дерева
func TestCreateNode(t *testing.T) {
	tree := &Tree{}
	
	t.Run("CreateNodeBasic", func(t *testing.T) {
		node := tree.CreateNode(42)
		
		if node == nil {
			t.Fatal("Созданный узел не должен быть nil")
		}
		if node.getValue() != 42 {
			t.Errorf("Ожидалось значение 42, получено %d", node.getValue())
		}
		if node.getColor() != RED {
			t.Error("Новый узел должен быть красным")
		}
		if node.getLeft() != nil {
			t.Error("Левый потомок нового узла должен быть nil")
		}
		if node.getRight() != nil {
			t.Error("Правый потомок нового узла должен быть nil")
		}
		if node.getParent() != nil {
			t.Error("Родитель нового узла должен быть nil")
		}
	})
	
	t.Run("CreateMultipleNodes", func(t *testing.T) {
		values := []int{10, 20, 30, 40, 50}
		nodes := make([]*Tree, len(values))
		
		for i, val := range values {
			nodes[i] = tree.CreateNode(val)
			if nodes[i].getValue() != val {
				t.Errorf("Ожидалось значение %d, получено %d", val, nodes[i].getValue())
			}
			if nodes[i].getColor() != RED {
				t.Error("Все новые узлы должны быть красными")
			}
		}
		
		// Проверяем, что узлы независимы
		for i := 0; i < len(nodes)-1; i++ {
			if nodes[i] == nodes[i+1] {
				t.Error("Узлы должны быть разными объектами")
			}
		}
	})
	
	t.Run("CreateNodeAndUseInTree", func(t *testing.T) {
		root := CreateRoot(100)
		defer func() {
			if root != nil {
				root = DeleteNode(root, 100)
				root = DeleteNode(root, 50)
				root = DeleteNode(root, 150)
			}
		}()
		
		// Создаем узлы вручную и добавляем их в дерево
		node50 := tree.CreateNode(50)
		node150 := tree.CreateNode(150)
		
		// Вручную добавляем узлы в дерево (имитируем логику AddLeaf)
		node50.parent = root
		node150.parent = root
		root.left = node50
		root.right = node150
		
		// Проверяем, что узлы добавились корректно
		if root.getLeft().getValue() != 50 {
			t.Error("Левый потомок должен иметь значение 50")
		}
		if root.getRight().getValue() != 150 {
			t.Error("Правый потомок должен иметь значение 150")
		}
		if node50.getParent() != root {
			t.Error("Родитель узла 50 должен быть корнем")
		}
		if node150.getParent() != root {
			t.Error("Родитель узла 150 должен быть корнем")
		}
	})
}
func TestRedBlackTreeCreateRoot(t *testing.T) {
	root := CreateRoot(10)
	defer func() {
		if root != nil {
			root = DeleteNode(root, 10)
		}
	}()
	
	if root == nil {
		t.Fatal("Корень не должен быть nil")
	}
	if root.getValue() != 10 {
		t.Errorf("Ожидалось значение 10, получено %d", root.getValue())
	}
	if root.getColor() != BLACK {
		t.Error("Корень должен быть черным")
	}
	if root.getLeft() != nil {
		t.Error("Левый потомок корня должен быть nil")
	}
	if root.getRight() != nil {
		t.Error("Правый потомок корня должен быть nil")
	}
	if root.getParent() != nil {
		t.Error("Родитель корня должен быть nil")
	}
}

func TestRedBlackTreeAddLeaf(t *testing.T) {
	root := CreateRoot(10)
	defer func() {
		if root != nil {
			root = DeleteNode(root, 10)
			root = DeleteNode(root, 5)
			root = DeleteNode(root, 15)
		}
	}()
	
	root = AddLeaf(root, 5)
	if !IfValueExists(root, 5) {
		t.Error("Значение 5 должно существовать в дереве")
	}
	
	root = AddLeaf(root, 15)
	if !IfValueExists(root, 15) {
		t.Error("Значение 15 должно существовать в дереве")
	}
	
	if root.getLeft() == nil {
		t.Error("Левый потомок не должен быть nil")
	}
	if root.getRight() == nil {
		t.Error("Правый потомок не должен быть nil")
	}
}

func TestRedBlackTreeSearch(t *testing.T) {
	root := CreateRoot(10)
	defer func() {
		if root != nil {
			root = DeleteNode(root, 10)
			root = DeleteNode(root, 5)
			root = DeleteNode(root, 15)
			root = DeleteNode(root, 3)
			root = DeleteNode(root, 7)
		}
	}()
	
	root = AddLeaf(root, 5)
	root = AddLeaf(root, 15)
	root = AddLeaf(root, 3)
	root = AddLeaf(root, 7)
	
	if !IfValueExists(root, 10) {
		t.Error("Значение 10 должно существовать в дереве")
	}
	if !IfValueExists(root, 5) {
		t.Error("Значение 5 должно существовать в дереве")
	}
	if !IfValueExists(root, 15) {
		t.Error("Значение 15 должно существовать в дереве")
	}
	if !IfValueExists(root, 3) {
		t.Error("Значение 3 должно существовать в дереве")
	}
	if !IfValueExists(root, 7) {
		t.Error("Значение 7 должно существовать в дереве")
	}
	if IfValueExists(root, 100) {
		t.Error("Значение 100 не должно существовать в дереве")
	}
}

func TestRedBlackTreeMinMax(t *testing.T) {
	root := CreateRoot(10)
	defer func() {
		if root != nil {
			root = DeleteNode(root, 10)
			root = DeleteNode(root, 5)
			root = DeleteNode(root, 15)
			root = DeleteNode(root, 1)
			root = DeleteNode(root, 20)
		}
	}()
	
	root = AddLeaf(root, 5)
	root = AddLeaf(root, 15)
	root = AddLeaf(root, 1)
	root = AddLeaf(root, 20)
	
	minNode := FindMinimum(root)
	maxNode := FindMax(root)
	
	if minNode == nil {
		t.Error("Минимальный узел не должен быть nil")
	} else if minNode.getValue() != 1 {
		t.Errorf("Ожидалось минимальное значение 1, получено %d", minNode.getValue())
	}
	
	if maxNode == nil {
		t.Error("Максимальный узел не должен быть nil")
	} else if maxNode.getValue() != 20 {
		t.Errorf("Ожидалось максимальное значение 20, получено %d", maxNode.getValue())
	}
}

func TestRedBlackTreeDeleteLeaf(t *testing.T) {
	root := CreateRoot(10)
	root = AddLeaf(root, 5)
	root = AddLeaf(root, 15)
	
	if !IfValueExists(root, 5) {
		t.Error("Значение 5 должно существовать в дереве перед удалением")
	}
	
	root = DeleteNode(root, 5)
	
	if IfValueExists(root, 5) {
		t.Error("Значение 5 не должно существовать в дереве после удаления")
	}
	if !IfValueExists(root, 10) {
		t.Error("Значение 10 должно существовать в дереве после удаления")
	}
	if !IfValueExists(root, 15) {
		t.Error("Значение 15 должно существовать в дереве после удаления")
	}
	
	// Очистка
	root = DeleteNode(root, 10)
	root = DeleteNode(root, 15)
}

func TestRedBlackTreeDeleteNodeWithOneChild(t *testing.T) {
	root := CreateRoot(10)
	root = AddLeaf(root, 5)
	root = AddLeaf(root, 15)
	root = AddLeaf(root, 3) // Левый ребенок для 5
	
	if !IfValueExists(root, 5) {
		t.Error("Значение 5 должно существовать в дереве перед удалением")
	}
	
	root = DeleteNode(root, 5)
	
	if IfValueExists(root, 5) {
		t.Error("Значение 5 не должно существовать в дереве после удаления")
	}
	if !IfValueExists(root, 3) {
		t.Error("Значение 3 должно существовать в дереве после удаление")
	}
	if !IfValueExists(root, 10) {
		t.Error("Значение 10 должно существовать в дереве после удаления")
	}
	if !IfValueExists(root, 15) {
		t.Error("Значение 15 должно существовать в дереве после удаления")
	}
	
	// Очистка
	root = DeleteNode(root, 10)
	root = DeleteNode(root, 15)
	root = DeleteNode(root, 3)
}

func TestRedBlackTreeDeleteNodeWithTwoChildren(t *testing.T) {
	root := CreateRoot(10)
	root = AddLeaf(root, 5)
	root = AddLeaf(root, 15)
	root = AddLeaf(root, 3)
	root = AddLeaf(root, 7)
	
	if !IfValueExists(root, 5) {
		t.Error("Значение 5 должно существовать в дереве перед удалением")
	}
	
	root = DeleteNode(root, 5)
	
	if IfValueExists(root, 5) {
		t.Error("Значение 5 не должно существовать в дереве после удаления")
	}
	if !IfValueExists(root, 3) {
		t.Error("Значение 3 должно существовать в дереве после удаления")
	}
	if !IfValueExists(root, 7) {
		t.Error("Значение 7 должно существовать в дереве после удаления")
	}
	if !IfValueExists(root, 10) {
		t.Error("Значение 10 должно существовать в дереве после удаления")
	}
	if !IfValueExists(root, 15) {
		t.Error("Значение 15 должно существовать в дереве после удаления")
	}
	
	// Очистка
	root = DeleteNode(root, 10)
	root = DeleteNode(root, 15)
	root = DeleteNode(root, 3)
	root = DeleteNode(root, 7)
}

func TestRedBlackTreeTraversals(t *testing.T) {
	root := CreateRoot(10)
	root = AddLeaf(root, 5)
	root = AddLeaf(root, 15)
	root = AddLeaf(root, 3)
	root = AddLeaf(root, 7)
	
	// Проверяем, что обходы не паникуют
	tree := &Tree{}
	
	t.Run("Symmetrical", func(t *testing.T) {
		defer func() {
			if r := recover(); r != nil {
				t.Errorf("Symmetrical обход вызвал панику: %v", r)
			}
		}()
		tree.Symmetrical(root)
	})
	
	t.Run("FromTopToBottom", func(t *testing.T) {
		defer func() {
			if r := recover(); r != nil {
				t.Errorf("FromTopToBottom обход вызвал панику: %v", r)
			}
		}()
		tree.FromTopToBottom(root)
	})
	
	t.Run("FromBottomToTop", func(t *testing.T) {
		defer func() {
			if r := recover(); r != nil {
				t.Errorf("FromBottomToTop обход вызвал панику: %v", r)
			}
		}()
		tree.FromBottomToTop(root)
	})
	
	t.Run("BFS", func(t *testing.T) {
		defer func() {
			if r := recover(); r != nil {
				t.Errorf("BFS обход вызвал панику: %v", r)
			}
		}()
		tree.BFS(root)
	})
	
	// Очистка
	root = DeleteNode(root, 10)
	root = DeleteNode(root, 5)
	root = DeleteNode(root, 15)
	root = DeleteNode(root, 3)
	root = DeleteNode(root, 7)
}

func TestRedBlackTreeHeight(t *testing.T) {
	root := CreateRoot(10)
	tree := &Tree{}
	
	height := tree.GetHeight(root)
	if height != 1 {
		t.Errorf("Ожидалась высота 1 для корня, получено %d", height)
	}
	
	root = AddLeaf(root, 5)
	root = AddLeaf(root, 15)
	
	height = tree.GetHeight(root)
	if height < 2 {
		t.Errorf("Ожидалась высота не менее 2 для дерева с детьми, получено %d", height)
	}
	
	// Очистка
	root = DeleteNode(root, 10)
	root = DeleteNode(root, 5)
	root = DeleteNode(root, 15)
}

func TestRedBlackTreeEmptyTree(t *testing.T) {
	var root *Tree
	
	// Поиск в пустом дереве
	if IfValueExists(root, 10) {
		t.Error("Поиск в пустом дереве должен возвращать false")
	}
	
	// Удаление из пустого дерева
	root = DeleteNode(root, 10)
	if root != nil {
		t.Error("Удаление из пустом дереве должно возвращать nil")
	}
	
	// Поиск минимума/максимума в пустом дереве
	minNode := FindMinimum(root)
	if minNode != nil {
		t.Error("Поиск минимума в пустом дереве должен возвращать nil")
	}
	
	maxNode := FindMax(root)
	if maxNode != nil {
		t.Error("Поиск максимума в пустом дереве должен возвращать nil")
	}
	
	// Обходы пустого дерева
	tree := &Tree{}
	tree.Symmetrical(root) // Не должно паниковать
	tree.FromTopToBottom(root)
	tree.FromBottomToTop(root)
	tree.BFS(root)
}

func TestRedBlackTreeColorProperties(t *testing.T) {
	root := CreateRoot(10)
	root = AddLeaf(root, 5)
	root = AddLeaf(root, 15)
	root = AddLeaf(root, 3)
	root = AddLeaf(root, 7)
	root = AddLeaf(root, 12)
	root = AddLeaf(root, 20)
	
	// Проверяем основные свойства красно-черного дерева
	// 1. Корень черный
	if root.getColor() != BLACK {
		t.Error("Корень должен быть черным")
	}
	
	// 2. Все листья (nil) считаются черными - это выполняется по определению
	
	// 3. Если узел красный, то оба его потомка черные
	checkRedBlackProperties(t, root)
	
	// Очистка
	root = DeleteNode(root, 10)
	root = DeleteNode(root, 5)
	root = DeleteNode(root, 15)
	root = DeleteNode(root, 3)
	root = DeleteNode(root, 7)
	root = DeleteNode(root, 12)
	root = DeleteNode(root, 20)
}

// Вспомогательная функция для проверки свойств красно-черного дерева
func checkRedBlackProperties(t *testing.T, node *Tree) {
	if node == nil {
		return
	}
	
	// Если узел красный, проверяем что оба потомка черные
	if node.getColor() == RED {
		if node.getLeft() != nil && node.getLeft().getColor() != BLACK {
			t.Errorf("Красный узел %d имеет красного левого потомка", node.getValue())
		}
		if node.getRight() != nil && node.getRight().getColor() != BLACK {
			t.Errorf("Красный узел %d имеет красного правого потомка", node.getValue())
		}
	}
	
	// Рекурсивно проверяем детей
	checkRedBlackProperties(t, node.getLeft())
	checkRedBlackProperties(t, node.getRight())
}

// TestFixInsert проверяет балансировку после вставки через публичный интерфейс
func TestFixInsert(t *testing.T) {
	// Тест 1: Вставка в пустое дерево
	root := CreateRoot(10)
	
	// Тест 2: Вставка с нарушением свойства (случай 2 - дядя красный)
	root = AddLeaf(root, 5)
	root = AddLeaf(root, 15)
	root = AddLeaf(root, 3) // Это может вызвать перекрашивание
	
	// Проверяем, что свойства сохранились
	checkRedBlackProperties(t, root)
	
	// Тест 3: Вставка, требующая левого поворота (случай 3-4)
	root2 := CreateRoot(10)
	root2 = AddLeaf(root2, 5)
	root2 = AddLeaf(root2, 3) // Это вызовет правый поворот
	
	checkRedBlackProperties(t, root2)
	
	// Тест 4: Вставка, требующая правого поворота (случай 3-4)
	root3 := CreateRoot(10)
	root3 = AddLeaf(root3, 15)
	root3 = AddLeaf(root3, 20) // Это вызовет левый поворот
	
	checkRedBlackProperties(t, root3)
	
	// Тест 5: Комплексный тест с несколькими вставками
	root4 := CreateRoot(10)
	values := []int{5, 15, 3, 7, 12, 18, 1, 4, 6, 8, 11, 13, 16, 19}
	for _, val := range values {
		root4 = AddLeaf(root4, val)
		// После каждой вставки проверяем свойства
		checkRedBlackProperties(t, root4)
	}
	
	// Очистка
	root = DeleteNode(root, 10)
	root = DeleteNode(root, 5)
	root = DeleteNode(root, 15)
	root = DeleteNode(root, 3)
	
	root2 = DeleteNode(root2, 10)
	root2 = DeleteNode(root2, 5)
	root2 = DeleteNode(root2, 3)
	
	root3 = DeleteNode(root3, 10)
	root3 = DeleteNode(root3, 15)
	root3 = DeleteNode(root3, 20)
	
	for _, val := range append([]int{10}, values...) {
		root4 = DeleteNode(root4, val)
	}
}

// TestFixInsertSpecificCases проверяет конкретные случаи балансировки через публичный интерфейс
func TestFixInsertSpecificCases(t *testing.T) {
	// Случай 1: Вставка в дерево с красным дядей
	t.Run("Case1_RedUncle", func(t *testing.T) {
		root := CreateRoot(10)
		root = AddLeaf(root, 5)
		root = AddLeaf(root, 15)
		root = AddLeaf(root, 3) // Это вызовет случай с красным дядей
		
		checkRedBlackProperties(t, root)
		
		// Очистка
		root = DeleteNode(root, 10)
		root = DeleteNode(root, 5)
		root = DeleteNode(root, 15)
		root = DeleteNode(root, 3)
	})
	
	// Случай 2: Вставка требующая левого поворота
	t.Run("Case2_LeftRotation", func(t *testing.T) {
		root := CreateRoot(10)
		root = AddLeaf(root, 15)
		root = AddLeaf(root, 20) // Это вызовет левый поворот
		
		checkRedBlackProperties(t, root)
		
		// Очистка
		root = DeleteNode(root, 10)
		root = DeleteNode(root, 15)
		root = DeleteNode(root, 20)
	})
	
	// Случай 3: Вставка требующая правого поворота
	t.Run("Case3_RightRotation", func(t *testing.T) {
		root := CreateRoot(10)
		root = AddLeaf(root, 5)
		root = AddLeaf(root, 3) // Это вызовет правый поворот
		
		checkRedBlackProperties(t, root)
		
		// Очистка
		root = DeleteNode(root, 10)
		root = DeleteNode(root, 5)
		root = DeleteNode(root, 3)
	})
}

// TestRotationThroughPublicInterface проверяет повороты через публичные операции вставки
func TestRotationThroughPublicInterface(t *testing.T) {
	// Тестируем, что повороты работают корректно через публичный интерфейс AddLeaf
	
	// Сценарий для правого поворота
	t.Run("RightRotationScenario", func(t *testing.T) {
		root := CreateRoot(30)
		root = AddLeaf(root, 20)
		root = AddLeaf(root, 10) // Должен вызвать правый поворот
		
		// После поворота корнем должен стать узел 20
		if root.getValue() != 20 {
			t.Errorf("После правого поворота корень должен быть 20, получено %d", root.getValue())
		}
		
		checkRedBlackProperties(t, root)
		
		// Очистка
		root = DeleteNode(root, 30)
		root = DeleteNode(root, 20)
		root = DeleteNode(root, 10)
	})
	
	// Сценарий для левого поворота
	t.Run("LeftRotationScenario", func(t *testing.T) {
		root := CreateRoot(10)
		root = AddLeaf(root, 20)
		root = AddLeaf(root, 30) // Должен вызвать левый поворот
		
		// После поворота корнем должен стать узел 20
		if root.getValue() != 20 {
			t.Errorf("После левого поворота корень должен быть 20, получено %d", root.getValue())
		}
		
		checkRedBlackProperties(t, root)
		
		// Очистка
		root = DeleteNode(root, 10)
		root = DeleteNode(root, 20)
		root = DeleteNode(root, 30)
	})
}

// TestComplexScenarios проверяет сложные сценарии работы дерева
func TestComplexScenarios(t *testing.T) {
	t.Run("SequentialInsertAndDelete", func(t *testing.T) {
		root := CreateRoot(50)
		
		// Последовательная вставка
		values := []int{30, 70, 20, 40, 60, 80, 10, 25, 35, 45, 55, 65, 75, 85}
		for _, val := range values {
			root = AddLeaf(root, val)
			checkRedBlackProperties(t, root)
		}
		
		// Последовательное удаление
		for _, val := range values {
			root = DeleteNode(root, val)
			checkRedBlackProperties(t, root)
		}
		
		// Удаление корня
		root = DeleteNode(root, 50)
		if root != nil {
			t.Error("После удаления всех узлов дерево должно быть пустым")
		}
	})
	
	t.Run("RandomOperations", func(t *testing.T) {
		root := CreateRoot(100)
		
		// Случайные операции вставки и удаления
		operations := []struct {
			value    int
			operation string // "add" or "delete"
		}{
			{50, "add"}, {150, "add"}, {25, "add"}, {75, "add"}, 
			{125, "add"}, {175, "add"}, {50, "delete"}, {25, "delete"},
			{200, "add"}, {10, "add"}, {30, "add"}, {60, "add"},
		}
		
		for _, op := range operations {
			if op.operation == "add" {
				root = AddLeaf(root, op.value)
			} else {
				root = DeleteNode(root, op.value)
			}
			checkRedBlackProperties(t, root)
		}
		
		// Очистка
		remaining := []int{100, 150, 75, 125, 175, 200, 10, 30, 60}
		for _, val := range remaining {
			root = DeleteNode(root, val)
		}
	})
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ДВУСВЯЗНЫЙ СПИСОК
func TestStudentGroupPrintForward(t *testing.T) {
	group := NewStudentGroup()
	defer group.Clear()

	// Тест пустого списка
	var output string
	// Захватываем вывод
	oldStdout := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	group.PrintForward()

	w.Close()
	os.Stdout = oldStdout
	buf, _ := io.ReadAll(r)
	output = string(buf)

	if output != "\n" {
		t.Errorf("Ожидался пустой вывод, получено '%s'", output)
	}

	// Тест с одним элементом
	group.AddStudentToEnd("Alice")
	
	r, w, _ = os.Pipe()
	os.Stdout = w
	group.PrintForward()
	w.Close()
	os.Stdout = oldStdout
	buf, _ = io.ReadAll(r)
	output = string(buf)

	expected := "Alice \n"
	if output != expected {
		t.Errorf("Ожидался вывод '%s', получено '%s'", expected, output)
	}

	// Тест с несколькими элементами
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	
	r, w, _ = os.Pipe()
	os.Stdout = w
	group.PrintForward()
	w.Close()
	os.Stdout = oldStdout
	buf, _ = io.ReadAll(r)
	output = string(buf)

	expected = "Alice Bob Charlie \n"
	if output != expected {
		t.Errorf("Ожидался вывод '%s', получено '%s'", expected, output)
	}
}

func TestStudentGroupPrintReverse(t *testing.T) {
	group := NewStudentGroup()
	defer group.Clear()

	// Тест пустого списка
	var output string
	oldStdout := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	group.PrintReverse()

	w.Close()
	os.Stdout = oldStdout
	buf, _ := io.ReadAll(r)
	output = string(buf)

	if output != "\n" {
		t.Errorf("Ожидался пустой вывод, получено '%s'", output)
	}

	// Тест с одним элементом
	group.AddStudentToEnd("Alice")
	
	r, w, _ = os.Pipe()
	os.Stdout = w
	group.PrintReverse()
	w.Close()
	os.Stdout = oldStdout
	buf, _ = io.ReadAll(r)
	output = string(buf)

	expected := "Alice \n"
	if output != expected {
		t.Errorf("Ожидался вывод '%s', получено '%s'", expected, output)
	}

	// Тест с несколькими элементами
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	
	r, w, _ = os.Pipe()
	os.Stdout = w
	group.PrintReverse()
	w.Close()
	os.Stdout = oldStdout
	buf, _ = io.ReadAll(r)
	output = string(buf)

	expected = "Charlie Bob Alice \n"
	if output != expected {
		t.Errorf("Ожидался вывод '%s', получено '%s'", expected, output)
	}
}

func TestStudentGroupPrintOperations(t *testing.T) {
	group := NewStudentGroupWithFirst("Middle")
	defer group.Clear()

	// Проверяем порядок после различных операций
	group.AddStudentToStart("First")
	group.AddStudentToEnd("Last")
	group.AddStudentAfter("Middle", "AfterMiddle")
	group.AddStudentBefore("Middle", "BeforeMiddle")

	// Проверяем прямой порядок
	oldStdout := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	group.PrintForward()

	w.Close()
	os.Stdout = oldStdout
	buf, _ := io.ReadAll(r)
	forwardOutput := string(buf)

	expectedForward := "First BeforeMiddle Middle AfterMiddle Last \n"
	if forwardOutput != expectedForward {
		t.Errorf("Ожидался прямой вывод '%s', получено '%s'", expectedForward, forwardOutput)
	}

	// Проверяем обратный порядок
	r, w, _ = os.Pipe()
	os.Stdout = w

	group.PrintReverse()

	w.Close()
	os.Stdout = oldStdout
	buf, _ = io.ReadAll(r)
	reverseOutput := string(buf)

	expectedReverse := "Last AfterMiddle Middle BeforeMiddle First \n"
	if reverseOutput != expectedReverse {
		t.Errorf("Ожидался обратный вывод '%s', получено '%s'", expectedReverse, reverseOutput)
	}
}
func TestStudentGroupConstructorAndBasicOperations(t *testing.T) {
	// ИСПРАВЛЕНО: используем NewStudentGroupWithFirst вместо NewStudentGroup
	group := NewStudentGroupWithFirst("Alice")
	defer group.Clear()
	
	if !group.Contains("Alice") {
		t.Error("Группа должна содержать 'Alice'")
	}
	if group.IsEmpty() {
		t.Error("Группа не должна быть пустой")
	}
	if group.GetFirst() != "Alice" {
		t.Errorf("Ожидался первый элемент 'Alice', получено '%s'", group.GetFirst())
	}
	if group.GetLast() != "Alice" {
		t.Errorf("Ожидался последний элемент 'Alice', получено '%s'", group.GetLast())
	}
}

func TestStudentGroupAddToEnd(t *testing.T) {
	group := NewStudentGroup()
	defer group.Clear()
	
	group.AddStudentToStart("Bob")
	group.AddStudentToEnd("Charlie")
	
	if group.GetFirst() != "Bob" {
		t.Errorf("Ожидался первый элемент 'Bob', получено '%s'", group.GetFirst())
	}
	if group.GetLast() != "Charlie" {
		t.Errorf("Ожидался последний элемент 'Charlie', получено '%s'", group.GetLast())
	}
}

func TestStudentGroupAddBeforeAndAfter(t *testing.T) {
	// ИСПРАВЛЕНО: используем NewStudentGroupWithFirst вместо NewStudentGroup
	group := NewStudentGroupWithFirst("Bob")
	defer group.Clear()
	
	group.AddStudentToEnd("Charlie")
	group.AddStudentBefore("Charlie", "Alice")
	group.AddStudentAfter("Bob", "David")
	
	if !group.Contains("Alice") {
		t.Error("Группа должна содержать 'Alice'")
	}
	if !group.Contains("David") {
		t.Error("Группа должна содержать 'David'")
	}
}

func TestStudentGroupAllDeleteTests(t *testing.T) {
	group := NewStudentGroup()
	defer group.Clear()
	
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	
	group.DeleteStudentFromStart()
	if group.GetFirst() != "Bob" {
		t.Errorf("Ожидался первый элемент 'Bob', получено '%s'", group.GetFirst())
	}
	
	group.DeleteStudentFromEnd()
	if group.GetLast() != "Bob" {
		t.Errorf("Ожидался последний элемент 'Bob', получено '%s'", group.GetLast())
	}
	
	group.AddStudentToEnd("Charlie")
	group.AddStudentToEnd("David")
	group.DeleteStudentAfter("Bob")
	if group.Contains("Charlie") {
		t.Error("Группа не должна содержать 'Charlie' после удаления")
	}
	
	group.AddStudentBefore("David", "Charlie")
	group.DeleteStudentBefore("David")
	if group.Contains("Charlie") {
		t.Error("Группа не должна содержать 'Charlie' после удаления")
	}
}
func TestStudentGroupDeleteFromEnd(t *testing.T) {
	// Тест 1: Удаление из пустого списка
	group := NewStudentGroup()
	group.DeleteStudentFromEnd() // Не должно быть паники

	if !group.IsEmpty() {
		t.Error("Пустой список должен остаться пустым после удаления")
	}
	if group.head != nil {
		t.Error("Head должен остаться nil после удаления из пустого списка")
	}
	if group.tail != nil {
		t.Error("Tail должен остаться nil после удаления из пустого списка")
	}

	// Тест 2: Удаление единственного элемента
	group.AddStudentToEnd("Alice")
	group.DeleteStudentFromEnd()

	if !group.IsEmpty() {
		t.Error("Список должен быть пустым после удаления единственного элемента")
	}
	if group.head != nil {
		t.Error("Head должен быть nil после удаления единственного элемента")
	}
	if group.tail != nil {
		t.Error("Tail должен быть nil после удаления единственного элемента")
	}
	if group.Contains("Alice") {
		t.Error("Группа не должна содержать 'Alice' после удаления")
	}

	// Тест 3: Удаление из списка с двумя элементами
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.DeleteStudentFromEnd()

	if group.IsEmpty() {
		t.Error("Список не должен быть пустым после удаления одного из двух элементов")
	}
	if group.Contains("Bob") {
		t.Error("Группа не должна содержать 'Bob' после удаления")
	}
	if !group.Contains("Alice") {
		t.Error("Группа должна содержать 'Alice' после удаления последнего элемента")
	}
	if group.GetFirst() != "Alice" {
		t.Errorf("Ожидался первый элемент 'Alice', получено '%s'", group.GetFirst())
	}
	if group.GetLast() != "Alice" {
		t.Errorf("Ожидался последний элемент 'Alice', получено '%s'", group.GetLast())
	}
	if group.head != group.tail {
		t.Error("Head и tail должны указывать на один узел в списке с одним элементом")
	}
	if group.head.next != nil {
		t.Error("Head.next должен быть nil в списке с одним элементом")
	}
	if group.head.prev != nil {
		t.Error("Head.prev должен быть nil в списке с одним элементом")
	}

	// Тест 4: Удаление из списка с тремя элементами
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	
	group.DeleteStudentFromEnd()

	if group.Contains("Charlie") {
		t.Error("Группа не должна содержать 'Charlie' после удаления")
	}
	if !group.Contains("Alice") {
		t.Error("Группа должна содержать 'Alice'")
	}
	if !group.Contains("Bob") {
		t.Error("Группа должна содержать 'Bob'")
	}
	if group.GetFirst() != "Alice" {
		t.Errorf("Ожидался первый элемент 'Alice', получено '%s'", group.GetFirst())
	}
	if group.GetLast() != "Bob" {
		t.Errorf("Ожидался последний элемент 'Bob', получено '%s'", group.GetLast())
	}
	// Проверяем связи
	if group.tail.next != nil {
		t.Error("Tail.next должен быть nil")
	}
	if group.tail.prev != group.head {
		t.Error("Tail.prev должен указывать на head")
	}
	if group.head.next != group.tail {
		t.Error("Head.next должен указывать на tail")
	}

	// Тест 5: Последовательное удаление всех элементов с конца
	group.Clear()
	group.AddStudentToEnd("First")
	group.AddStudentToEnd("Second")
	group.AddStudentToEnd("Third")
	
	// Удаляем Third
	group.DeleteStudentFromEnd()
	if group.GetLast() != "Second" {
		t.Errorf("Ожидался последний элемент 'Second', получено '%s'", group.GetLast())
	}
	
	// Удаляем Second
	group.DeleteStudentFromEnd()
	if group.GetLast() != "First" {
		t.Errorf("Ожидался последний элемент 'First', получено '%s'", group.GetLast())
	}
	
	// Удаляем First
	group.DeleteStudentFromEnd()
	if !group.IsEmpty() {
		t.Error("Список должен быть пустым после удаления всех элементов")
	}
	if group.head != nil {
		t.Error("Head должен быть nil после удаления всех элементов")
	}
	if group.tail != nil {
		t.Error("Tail должен быть nil после удаления всех элементов")
	}

	// Тест 6: Чередование добавлений и удалений с конца
	group.AddStudentToEnd("A")
	group.AddStudentToEnd("B")
	group.DeleteStudentFromEnd()
	
	if group.GetLast() != "A" {
		t.Errorf("Ожидался последний элемент 'A', получено '%s'", group.GetLast())
	}
	
	group.AddStudentToEnd("C")
	group.AddStudentToEnd("D")
	group.DeleteStudentFromEnd()
	
	if group.GetLast() != "C" {
		t.Errorf("Ожидался последний элемент 'C', получено '%s'", group.GetLast())
	}
	
	group.DeleteStudentFromEnd()
	if group.GetLast() != "A" {
		t.Errorf("Ожидался последний элемент 'A', получено '%s'", group.GetLast())
	}

	// Тест 7: Проверка целостности связей после удаления
	group.Clear()
	group.AddStudentToEnd("X")
	group.AddStudentToEnd("Y") 
	group.AddStudentToEnd("Z")
	
	group.DeleteStudentFromEnd()
	
	// Проверяем что связи между X и Y корректны
	if group.head.student != "X" {
		t.Errorf("Head должен быть 'X', получено '%s'", group.head.student)
	}
	if group.tail.student != "Y" {
		t.Errorf("Tail должен быть 'Y', получено '%s'", group.tail.student)
	}
	if group.head.next != group.tail {
		t.Error("Head.next должен указывать на tail")
	}
	if group.tail.prev != group.head {
		t.Error("Tail.prev должен указывать на head")
	}
	if group.head.prev != nil {
		t.Error("Head.prev должен быть nil")
	}
	if group.tail.next != nil {
		t.Error("Tail.next должен быть nil")
	}
}
func TestStudentGroupDeleteBefore(t *testing.T) {
	// Тест 1: Удаление из пустого списка
	group := NewStudentGroup()
	group.DeleteStudentBefore("Nonexistent") // Не должно быть паники

	if !group.IsEmpty() {
		t.Error("Пустой список должен остаться пустым")
	}

	// Тест 2: Удаление когда target не найден
	group.AddStudentToEnd("Alice")
	group.DeleteStudentBefore("Nonexistent")

	if !group.Contains("Alice") {
		t.Error("Элемент не должен быть удален когда target не найден")
	}
	if group.GetFirst() != "Alice" {
		t.Errorf("Ожидался первый элемент 'Alice', получено '%s'", group.GetFirst())
	}

	// Тест 3: Удаление когда перед target нет элемента (target - первый)
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.DeleteStudentBefore("Alice") // Не должно ничего удалить

	if !group.Contains("Alice") {
		t.Error("Alice не должна быть удалена")
	}
	if !group.Contains("Bob") {
		t.Error("Bob не должен быть удален")
	}
	if group.GetFirst() != "Alice" {
		t.Errorf("Ожидался первый элемент 'Alice', получено '%s'", group.GetFirst())
	}

	// Тест 4: Удаление единственного элемента перед target (из середины)
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	
	group.DeleteStudentBefore("Charlie") // Удаляем Bob

	if group.Contains("Bob") {
		t.Error("Группа не должна содержать 'Bob' после удаления")
	}
	if !group.Contains("Alice") {
		t.Error("Группа должна содержать 'Alice'")
	}
	if !group.Contains("Charlie") {
		t.Error("Группа должна содержать 'Charlie'")
	}
	if group.GetFirst() != "Alice" {
		t.Errorf("Ожидался первый элемент 'Alice', получено '%s'", group.GetFirst())
	}
	if group.GetLast() != "Charlie" {
		t.Errorf("Ожидался последний элемент 'Charlie', получено '%s'", group.GetLast())
	}
	// Проверяем связи
	if group.head.next != group.tail {
		t.Error("Head.next должен указывать на tail")
	}
	if group.tail.prev != group.head {
		t.Error("Tail.prev должен указывать на head")
	}

	// Тест 5: Удаление первого элемента (когда удаляемый - head)
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	
	group.DeleteStudentBefore("Bob") // Удаляем Alice

	if group.Contains("Alice") {
		t.Error("Группа не должна содержать 'Alice' после удаления")
	}
	if !group.Contains("Bob") {
		t.Error("Группа должна содержать 'Bob'")
	}
	if !group.Contains("Charlie") {
		t.Error("Группа должна содержать 'Charlie'")
	}
	if group.GetFirst() != "Bob" {
		t.Errorf("Ожидался первый элемент 'Bob', получено '%s'", group.GetFirst())
	}
	if group.head.prev != nil {
		t.Error("Новый head не должен иметь предыдущего элемента")
	}
	if group.head.next != group.tail {
		t.Error("Head.next должен указывать на tail")
	}

	// Тест 6: Удаление когда удаляемый элемент является tail
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	
	// Сначала добавим элемент перед Charlie, чтобы Bob стал tail после удаления
	group.DeleteStudentBefore("Charlie") // Удаляем Bob, теперь Alice-Charlie

	if group.tail.student != "Charlie" {
		t.Errorf("Tail должен быть 'Charlie', получено '%s'", group.tail.student)
	}
	if group.head.student != "Alice" {
		t.Errorf("Head должен быть 'Alice', получено '%s'", group.head.student)
	}

	// Тест 7: Удаление из списка с двумя элементами (удаление head)
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	
	group.DeleteStudentBefore("Bob") // Удаляем Alice

	if group.Contains("Alice") {
		t.Error("Группа не должна содержать 'Alice' после удаления")
	}
	if !group.Contains("Bob") {
		t.Error("Группа должна содержать 'Bob'")
	}
	if group.GetFirst() != "Bob" {
		t.Errorf("Ожидался первый элемент 'Bob', получено '%s'", group.GetFirst())
	}
	if group.GetLast() != "Bob" {
		t.Errorf("Ожидался последний элемент 'Bob', получено '%s'", group.GetLast())
	}
	if group.head != group.tail {
		t.Error("Head и tail должны быть равны для одного элемента")
	}

	// Тест 8: Множественные удаления before
	group.Clear()
	group.AddStudentToEnd("A")
	group.AddStudentToEnd("B")
	group.AddStudentToEnd("C")
	group.AddStudentToEnd("D")
	group.AddStudentToEnd("E")
	
	group.DeleteStudentBefore("C") // Удаляем B, теперь A-C-D-E
	if group.Contains("B") {
		t.Error("Группа не должна содержать 'B' после удаления")
	}
	
	group.DeleteStudentBefore("D") // Удаляем C, теперь A-D-E
	if group.Contains("C") {
		t.Error("Группа не должна содержать 'C' после удаления")
	}
	
	group.DeleteStudentBefore("E") // Удаляем D, теперь A-E
	if group.Contains("D") {
		t.Error("Группа не должна содержать 'D' после удаления")
	}

	if !group.Contains("A") {
		t.Error("Группа должна содержать 'A'")
	}
	if !group.Contains("E") {
		t.Error("Группа должна содержать 'E'")
	}
	if group.GetFirst() != "A" {
		t.Errorf("Ожидался первый элемент 'A', получено '%s'", group.GetFirst())
	}
	if group.GetLast() != "E" {
		t.Errorf("Ожидался последний элемент 'E', получено '%s'", group.GetLast())
	}

	// Тест 9: Проверка целостности связей после удаления из середины
	group.Clear()
	group.AddStudentToEnd("X")
	group.AddStudentToEnd("Y")
	group.AddStudentToEnd("Z")
	
	group.DeleteStudentBefore("Z") // Удаляем Y

	// Проверяем что связи между X и Z корректны
	if group.head.student != "X" {
		t.Errorf("Head должен быть 'X', получено '%s'", group.head.student)
	}
	if group.tail.student != "Z" {
		t.Errorf("Tail должен быть 'Z', получено '%s'", group.tail.student)
	}
	if group.head.next != group.tail {
		t.Error("Head.next должен указывать на tail")
	}
	if group.tail.prev != group.head {
		t.Error("Tail.prev должен указывать на head")
	}
	if group.head.prev != nil {
		t.Error("Head.prev должен быть nil")
	}
	if group.tail.next != nil {
		t.Error("Tail.next должен быть nil")
	}

	// Тест 10: Удаление когда target - второй элемент (удаление head)
	group.Clear()
	group.AddStudentToEnd("First")
	group.AddStudentToEnd("Second")
	group.AddStudentToEnd("Third")
	
	group.DeleteStudentBefore("Second") // Удаляем First

	if group.Contains("First") {
		t.Error("Группа не должна содержать 'First' после удаления")
	}
	if group.GetFirst() != "Second" {
		t.Errorf("Ожидался первый элемент 'Second', получено '%s'", group.GetFirst())
	}
	if group.head.prev != nil {
		t.Error("Новый head не должен иметь предыдущего элемента")
	}
}
func TestStudentGroupDeleteAfter(t *testing.T) {
	// Тест 1: Удаление из пустого списка
	group := NewStudentGroup()
	group.DeleteStudentAfter("Nonexistent")
	if !group.IsEmpty() {
		t.Error("Пустой список должен остаться пустым")
	}

	// Тест 2: Target не найден
	group.AddStudentToEnd("Alice")
	group.DeleteStudentAfter("Nonexistent")
	if !group.Contains("Alice") || group.IsEmpty() {
		t.Error("Список не должен измениться когда target не найден")
	}

	// Тест 3: После target нет элемента (target - последний)
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.DeleteStudentAfter("Bob")
	if !group.Contains("Alice") || !group.Contains("Bob") {
		t.Error("Список не должен измениться когда после target нет элемента")
	}

	// Тест 4: Удаление из середины списка
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	group.DeleteStudentAfter("Alice") // Удаляем Bob

	if group.Contains("Bob") {
		t.Error("Bob должен быть удален")
	}
	if !group.Contains("Alice") || !group.Contains("Charlie") {
		t.Error("Alice и Charlie должны остаться в списке")
	}
	if group.head.next != group.tail {
		t.Error("Alice.next должен указывать на Charlie")
	}
	if group.tail.prev != group.head {
		t.Error("Charlie.prev должен указывать на Alice")
	}

	// Тест 5: Удаление последнего элемента
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.DeleteStudentAfter("Alice") // Удаляем Bob

	if group.Contains("Bob") {
		t.Error("Bob должен быть удален")
	}
	if group.GetLast() != "Alice" {
		t.Errorf("Последний элемент должен быть 'Alice', получено '%s'", group.GetLast())
	}
	if group.tail != group.head {
		t.Error("Tail должен быть равен Head для одного элемента")
	}
	if group.head.next != nil {
		t.Error("Head.next должен быть nil")
	}

	// Тест 6: Удаление когда удаляемый элемент является tail
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	group.DeleteStudentAfter("Bob") // Удаляем Charlie

	if group.Contains("Charlie") {
		t.Error("Charlie должен быть удален")
	}
	if group.GetLast() != "Bob" {
		t.Errorf("Последний элемент должен быть 'Bob', получено '%s'", group.GetLast())
	}
	if group.tail.next != nil {
		t.Error("Tail.next должен быть nil")
	}

	// Тест 7: Множественные удаления after
	group.Clear()
	group.AddStudentToEnd("A")
	group.AddStudentToEnd("B")
	group.AddStudentToEnd("C")
	group.AddStudentToEnd("D")
	
	group.DeleteStudentAfter("A") // Удаляем B, теперь A-C-D
	group.DeleteStudentAfter("A") // Удаляем C, теперь A-D
	group.DeleteStudentAfter("A") // Удаляем D, теперь A

	if group.Contains("B") || group.Contains("C") || group.Contains("D") {
		t.Error("B, C, D должны быть удалены")
	}
	if group.GetFirst() != "A" || group.GetLast() != "A" {
		t.Error("A должен быть единственным элементом")
	}

	// Тест 8: Проверка целостности связей
	group.Clear()
	group.AddStudentToEnd("X")
	group.AddStudentToEnd("Y")
	group.AddStudentToEnd("Z")
	group.DeleteStudentAfter("X") // Удаляем Y

	if group.head.next != group.tail {
		t.Error("X.next должен указывать на Z")
	}
	if group.tail.prev != group.head {
		t.Error("Z.prev должен указывать на X")
	}
	if group.head.prev != nil {
		t.Error("X.prev должен быть nil")
	}
	if group.tail.next != nil {
		t.Error("Z.next должен быть nil")
	}
}
func TestStudentGroupDeleteBeforeAfterEdgeCases(t *testing.T) {
	// Тест специальных граничных случаев для обоих методов
	
	// Случай 1: Удаление в списке с двумя элементами
	group := NewStudentGroup()
	group.AddStudentToEnd("First")
	group.AddStudentToEnd("Second")
	
	// DeleteAfter на первом элементе (удаление второго)
	group.DeleteStudentAfter("First")
	if group.Contains("Second") {
		t.Error("Second должен быть удален")
	}
	if group.GetFirst() != "First" || group.GetLast() != "First" {
		t.Error("First должен быть единственным элементом")
	}

	// Случай 2: DeleteBefore на втором элементе (удаление первого)
	group.Clear()
	group.AddStudentToEnd("First")
	group.AddStudentToEnd("Second")
	group.DeleteStudentBefore("Second")
	if group.Contains("First") {
		t.Error("First должен быть удален")
	}
	if group.GetFirst() != "Second" || group.GetLast() != "Second" {
		t.Error("Second должен быть единственным элементом")
	}

	// Случай 3: Последовательные вызовы на одном target
	group.Clear()
	group.AddStudentToEnd("A")
	group.AddStudentToEnd("B")
	group.AddStudentToEnd("C")
	
	group.DeleteStudentAfter("A") // Удаляем B
	group.DeleteStudentAfter("A") // Удаляем C
	group.DeleteStudentAfter("A") // Ничего не удаляет
	
	if group.Contains("B") || group.Contains("C") {
		t.Error("B и C должны быть удалены")
	}
	if group.GetFirst() != "A" || group.GetLast() != "A" {
		t.Error("A должен быть единственным элементом")
	}

	// Случай 4: Проверка обновления tail в сложном сценарии
	group.Clear()
	group.AddStudentToEnd("X")
	group.AddStudentToEnd("Y")
	group.AddStudentToEnd("Z")
	
	group.DeleteStudentAfter("Y") // Удаляем Z, tail становится Y
	if group.GetLast() != "Y" {
		t.Errorf("Последний элемент должен быть 'Y', получено '%s'", group.GetLast())
	}
	if group.tail.next != nil {
		t.Error("Tail.next должен быть nil")
	}

	group.DeleteStudentBefore("Y") // Удаляем X, теперь только Y
	if group.GetFirst() != "Y" || group.GetLast() != "Y" {
		t.Error("Y должен быть единственным элементом")
	}
	if group.head != group.tail {
		t.Error("Head и tail должны быть равны")
	}
}
func TestStudentGroupDeleteByValue(t *testing.T) {
	group := NewStudentGroup()
	defer group.Clear()

	// Тест 1: Удаление из пустого списка
	err := group.DeleteStudentByValue("Nonexistent")
	if err == nil {
		t.Error("Ожидалась ошибка при удалении из пустого списка")
	}
	expectedError := "нельзя удалить несуществующий элемент"
	if err.Error() != expectedError {
		t.Errorf("Ожидалась ошибка '%s', получено '%s'", expectedError, err.Error())
	}

	// Тест 2: Удаление несуществующего элемента из непустого списка
	group.AddStudentToEnd("Alice")
	err = group.DeleteStudentByValue("Bob")
	if err == nil {
		t.Error("Ожидалась ошибка при удалении несуществующего элемента")
	}
	if err.Error() != expectedError {
		t.Errorf("Ожидалась ошибка '%s', получено '%s'", expectedError, err.Error())
	}

	// Тест 3: Удаление единственного элемента
	group.Clear()
	group.AddStudentToEnd("Alice")
	
	err = group.DeleteStudentByValue("Alice")
	if err != nil {
		t.Errorf("Не ожидалась ошибка при удалении существующего элемента: %s", err)
	}
	if group.Contains("Alice") {
		t.Error("Группа не должна содержать 'Alice' после удаления")
	}
	if !group.IsEmpty() {
		t.Error("Группа должна быть пустой после удаления единственного элемента")
	}
	if group.head != nil {
		t.Error("Head должен быть nil после удаления единственного элемента")
	}
	if group.tail != nil {
		t.Error("Tail должен быть nil после удаления единственного элемента")
	}

	// Тест 4: Удаление первого элемента из списка с несколькими элементами
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	
	err = group.DeleteStudentByValue("Alice")
	if err != nil {
		t.Errorf("Не ожидалась ошибка при удалении первого элемента: %s", err)
	}
	if group.Contains("Alice") {
		t.Error("Группа не должна содержать 'Alice' после удаления")
	}
	if !group.Contains("Bob") {
		t.Error("Группа должна содержать 'Bob'")
	}
	if !group.Contains("Charlie") {
		t.Error("Группа должна содержать 'Charlie'")
	}
	if group.GetFirst() != "Bob" {
		t.Errorf("Ожидался первый элемент 'Bob', получено '%s'", group.GetFirst())
	}
	if group.head.prev != nil {
		t.Error("У нового head не должно быть предыдущего элемента")
	}

	// Тест 5: Удаление последнего элемента
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	
	err = group.DeleteStudentByValue("Charlie")
	if err != nil {
		t.Errorf("Не ожидалась ошибка при удалении последнего элемента: %s", err)
	}
	if group.Contains("Charlie") {
		t.Error("Группа не должна содержать 'Charlie' после удаления")
	}
	if !group.Contains("Alice") {
		t.Error("Группа должна содержать 'Alice'")
	}
	if !group.Contains("Bob") {
		t.Error("Группа должна содержать 'Bob'")
	}
	if group.GetLast() != "Bob" {
		t.Errorf("Ожидался последний элемент 'Bob', получено '%s'", group.GetLast())
	}
	if group.tail.next != nil {
		t.Error("У нового tail не должно быть следующего элемента")
	}

	// Тест 6: Удаление среднего элемента
	group.Clear()
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	group.AddStudentToEnd("Charlie")
	
	err = group.DeleteStudentByValue("Bob")
	if err != nil {
		t.Errorf("Не ожидалась ошибка при удалении среднего элемента: %s", err)
	}
	if group.Contains("Bob") {
		t.Error("Группа не должна содержать 'Bob' после удаления")
	}
	if !group.Contains("Alice") {
		t.Error("Группа должна содержать 'Alice'")
	}
	if !group.Contains("Charlie") {
		t.Error("Группа должна содержать 'Charlie'")
	}
	if group.GetFirst() != "Alice" {
		t.Errorf("Ожидался первый элемент 'Alice', получено '%s'", group.GetFirst())
	}
	if group.GetLast() != "Charlie" {
		t.Errorf("Ожидался последний элемент 'Charlie', получено '%s'", group.GetLast())
	}

	// Проверяем связи между оставшимися элементами
	if group.head.next != group.tail {
		t.Error("Head.next должен указывать на tail")
	}
	if group.tail.prev != group.head {
		t.Error("Tail.prev должен указывать на head")
	}

	// Тест 7: Удаление всех элементов по одному
	group.Clear()
	group.AddStudentToEnd("First")
	group.AddStudentToEnd("Second")
	group.AddStudentToEnd("Third")
	
	// Удаляем в произвольном порядке
	group.DeleteStudentByValue("Second")
	group.DeleteStudentByValue("First")
	group.DeleteStudentByValue("Third")
	
	if !group.IsEmpty() {
		t.Error("Группа должна быть пустой после удаления всех элементов")
	}
	if group.head != nil {
		t.Error("Head должен быть nil после удаления всех элементов")
	}
	if group.tail != nil {
		t.Error("Tail должен быть nil после удаления всех элементов")
	}

	// Тест 8: Удаление элемента, когда он встречается несколько раз (удаляется первый найденный)
	group.Clear()
	group.AddStudentToEnd("Duplicate")
	group.AddStudentToEnd("Other")
	group.AddStudentToEnd("Duplicate")
	
	err = group.DeleteStudentByValue("Duplicate")
	if err != nil {
		t.Errorf("Не ожидалась ошибка при удалении дубликата: %s", err)
	}
	// Должен удалиться только первый "Duplicate"
	if !group.Contains("Duplicate") {
		t.Error("Группа должна содержать второй 'Duplicate'")
	}
	if !group.Contains("Other") {
		t.Error("Группа должна содержать 'Other'")
	}
	if group.GetFirst() != "Other" {
		t.Errorf("Ожидался первый элемент 'Other', получено '%s'", group.GetFirst())
	}
}

func TestStudentGroupClearAndEmpty(t *testing.T) {
	group := NewStudentGroup()
	
	group.AddStudentToEnd("Alice")
	group.AddStudentToEnd("Bob")
	if group.IsEmpty() {
		t.Error("Группа не должна быть пустой")
	}
	
	group.Clear()
	if !group.IsEmpty() {
		t.Error("Группа должна быть пустой после очистки")
	}
}
