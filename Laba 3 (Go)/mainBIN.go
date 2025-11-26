package main

import (
	"bufio"
	"encoding/binary"
	"flag"
	"fmt"
	"os"
	"strconv"
	"strings"
)

// Глобальные maps для хранения структур данных
var (
	queues     = make(map[string]*Queue)
	stacks     = make(map[string]*Stack)
	lists      = make(map[string]*OdSpis)
	arrays     = make(map[string]*Array)
	groups     = make(map[string]*StudentGroup)
	trees      = make(map[string]*Tree)
	hashTables = make(map[string]*DoubleHashTable)
)

func main() {
	// Определяем флаги командной строки
	fileFlag := flag.String("file", "data.bin", "Файл для сохранения/загрузки данных")
	queryFlag := flag.String("query", "", "Команда для выполнения")
	flag.Parse()

	filename := *fileFlag
	query := *queryFlag

	// Загружаем данные при старте
	loadFromFile(filename)

	if query != "" {
		// Режим выполнения одной команды
		result := processQuery(query, filename)
		fmt.Println(result)
	} else {
		// Интерактивный режим
		fmt.Println("Программа запущена. Введите команды (для выхода введите 'exit'):")
		scanner := bufio.NewScanner(os.Stdin)
		for {
			fmt.Print("> ")
			if !scanner.Scan() {
				break
			}

			query := scanner.Text()
			if query == "exit" {
				break
			}

			result := processQuery(query, filename)
			fmt.Println(result)
		}
	}

	// Сохраняем данные при выходе
	saveToFile(filename)
	fmt.Println("Программа завершена.")
}

// Все остальные функции остаются без изменений (HCREATE, HADD, TADD, TSEARCH, LPUSHBEGIN, MCREATE и т.д.)
// ... [все остальные функции из предыдущего кода] ...

// Функции для работы с хэш-таблицами
func HCREATE(tableName string) string {
	if _, exists := hashTables[tableName]; exists {
		hashTables[tableName] = NewDoubleHashTable()
	}
	hashTables[tableName] = NewDoubleHashTable()
	return tableName
}

func HADD(tableName string, key int, value string) string {
	if _, exists := hashTables[tableName]; !exists {
		return "Хэш-таблица не найдена"
	}
	success := hashTables[tableName].ADD(key, value)
	if success {
		return value
	}
	return "Ошибка добавления (возможно, ключ уже существует)"
}

func HDEL(tableName string, key int, value string) string {
	if _, exists := hashTables[tableName]; !exists {
		return "Хэш-таблица не найдена"
	}
	success := hashTables[tableName].SETDEL(key, value)
	if success {
		return "Элемент удален"
	}
	return "Элемент не найден"
}

func HSEARCH(tableName string, key int, value string) string {
	if _, exists := hashTables[tableName]; !exists {
		return "Хэш-таблица не найдена"
	}
	found := hashTables[tableName].SET_AT(key, value)
	if found {
		return "Найден"
	}
	return "Не найден"
}

func HGET(tableName string) string {
	if _, exists := hashTables[tableName]; !exists {
		return "Хэш-таблица не найдена"
	}

	keys, values, count := hashTables[tableName].GetAllElements()
	if count == 0 {
		return "Хэш-таблица пустая"
	}

	var output strings.Builder
	for i := 0; i < count; i++ {
		output.WriteString(fmt.Sprintf("%d:%s", keys[i], values[i]))
		if i < count-1 {
			output.WriteString(" ")
		}
	}
	return output.String()
}

func HSIZE(tableName string) string {
	if _, exists := hashTables[tableName]; !exists {
		return "Хэш-таблица не найдена"
	}
	return strconv.Itoa(hashTables[tableName].ElementCount())
}

// Функции для работы с деревьями
func TADD(treeName string, value int) string {
	if _, exists := trees[treeName]; !exists {
		trees[treeName] = CreateRoot(value)
		return strconv.Itoa(value)
	} else {
		trees[treeName] = AddLeaf(trees[treeName], value)
		return strconv.Itoa(value)
	}
}

func TSEARCH(treeName string, value int) string {
	if _, exists := trees[treeName]; !exists {
		return "Дерево не найдено"
	}
	found := IfValueExists(trees[treeName], value)
	if found {
		return "Найден"
	}
	return "Не найден"
}

func TDELETE(treeName string, value int) string {
	if _, exists := trees[treeName]; !exists {
		return "Дерево не найдено"
	}
	tempTree := &Tree{}
	treePtr := trees[treeName]
	success := tempTree.deleteLeaf(&treePtr, value)
	if success {
		trees[treeName] = treePtr
		return "Элемент удален"
	}
	return "Элемент не найден или невозможно удалить"
}

func TGET(treeName string) string {
	if _, exists := trees[treeName]; !exists {
		return "Дерево не найдено"
	}
	if trees[treeName] == nil {
		return "Дерево пустое"
	}

	var output strings.Builder
	tempTree := &Tree{}
	oldStdout := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	tempTree.BFS(trees[treeName])

	w.Close()
	os.Stdout = oldStdout

	buf := make([]byte, 1024)
	n, _ := r.Read(buf)
	output.Write(buf[:n])

	result := output.String()
	if len(result) > 0 && result[len(result)-1] == '\n' {
		result = result[:len(result)-1]
	}
	return result
}

func TGETSYM(treeName string) string {
	if _, exists := trees[treeName]; !exists {
		return "Дерево не найдено"
	}
	if trees[treeName] == nil {
		return "Дерево пустое"
	}

	var output strings.Builder
	tempTree := &Tree{}
	oldStdout := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	tempTree.Symmetrical(trees[treeName])

	w.Close()
	os.Stdout = oldStdout

	buf := make([]byte, 1024)
	n, _ := r.Read(buf)
	output.Write(buf[:n])

	result := output.String()
	if len(result) > 0 && result[len(result)-1] == '\n' {
		result = result[:len(result)-1]
	}
	return result
}

func TDEL(treeName string, value int) string {
	if _, exists := trees[treeName]; !exists {
		return "Дерево не найдено"
	} else {
		tempTree := &Tree{}
		treePtr := trees[treeName]
		success := tempTree.deleteLeaf(&treePtr, value)
		if success {
			trees[treeName] = treePtr
			return strconv.Itoa(value)
		}
		return "Элемент не найден или невозможно удалить"
	}
}

func printTreeStructure(node *Tree, output *strings.Builder, prefix string, isLeft bool) {
	if node == nil {
		return
	}
	output.WriteString(prefix)
	if isLeft {
		output.WriteString("├----- ")
	} else {
		output.WriteString("└----- ")
	}
	colorStr := "r"
	if node.getColor() == BLACK {
		colorStr = "b"
	}
	output.WriteString(fmt.Sprintf("%d%s\n", node.getValue(), colorStr))

	newPrefix := prefix
	if isLeft {
		newPrefix += "│   "
	} else {
		newPrefix += "    "
	}
	printTreeStructure(node.getLeft(), output, newPrefix, true)
	printTreeStructure(node.getRight(), output, newPrefix, false)
}

func TREAD(treeName string) string {
	if _, exists := trees[treeName]; !exists {
		return "Дерево не найдено"
	}
	if trees[treeName] == nil {
		return "Дерево пустое"
	}

	var output strings.Builder
	printTreeStructure(trees[treeName], &output, "", true)

	result := output.String()
	if len(result) > 0 && result[len(result)-1] == '\n' {
		result = result[:len(result)-1]
	}
	return result
}

// Функции для работы с двусвязными списками
func LPOISK(groupName, str string) string {
	if _, exists := groups[groupName]; !exists {
		return "Двусвязный список не найден"
	}
	found := groups[groupName].Contains(str)
	if found {
		return "Найден"
	}
	return "Не найден"
}

func LPOPVALUE(groupName, str string) string {
	if _, exists := groups[groupName]; !exists {
		return "Двусвязный список не найден"
	}
	if !groups[groupName].Contains(str) {
		return "Элемент в двусвязном списке не найден"
	}
	err := groups[groupName].DeleteStudentByValue(str)
	if err != nil {
		return err.Error()
	}
	return "Элемент удален"
}

func LPUSHBEGIN(groupName, str string) string {
	if _, exists := groups[groupName]; !exists {
		groups[groupName] = NewStudentGroupWithFirst(str)
	} else {
		groups[groupName].AddStudentToStart(str)
	}
	return str
}

func LPUSHEND(groupName, str string) string {
	if _, exists := groups[groupName]; !exists {
		groups[groupName] = NewStudentGroupWithFirst(str)
	} else {
		groups[groupName].AddStudentToEnd(str)
	}
	return str
}

func LPUSHBEFORE(groupName, targetElement, str string) string {
	if _, exists := groups[groupName]; !exists {
		return "Двусвязный список не найден"
	}
	if !groups[groupName].Contains(targetElement) {
		return "Элемент в двусвязном списке не найден"
	}
	groups[groupName].AddStudentBefore(targetElement, str)
	return str
}

func LPUSHAFTER(groupName, pointerElement, str string) string {
	if _, exists := groups[groupName]; !exists {
		return "Двусвязный список не найден"
	}
	if !groups[groupName].Contains(pointerElement) {
		return "Элемент в двусвязном списке не найден"
	}
	groups[groupName].AddStudentAfter(pointerElement, str)
	return str
}

func LPOPBEFORE(groupName, str string) string {
	if _, exists := groups[groupName]; !exists {
		return "Двусвязный список не найден"
	}
	if !groups[groupName].Contains(str) {
		return "Элемент в двусвязном списке не найден"
	}
	if groups[groupName].GetFirst() == str {
		return "Выше головы не прыгнешь, рубить нечего"
	}
	groups[groupName].DeleteStudentBefore(str)
	return "Элемент удален"
}

func LPOPAFTER(groupName, str string) string {
	if _, exists := groups[groupName]; !exists {
		return "Двусвязный список не найден"
	}
	if !groups[groupName].Contains(str) {
		return "Элемент в двусвязном списке не найден"
	}
	if groups[groupName].GetLast() == str {
		return "За последним элементом ничего нет, удалять нечего"
	}
	groups[groupName].DeleteStudentAfter(str)
	return "Элемент удален"
}

func LPOPBEGIN(groupName string) string {
	if _, exists := groups[groupName]; !exists {
		return "Двусвязный список не найден"
	}
	if groups[groupName].IsEmpty() {
		return "Двусвязный список пустой"
	}
	groups[groupName].DeleteStudentFromStart()
	return "Элемент удален"
}

func LPOPEND(groupName string) string {
	if _, exists := groups[groupName]; !exists {
		return "Двусвязный список не найден"
	}
	if groups[groupName].IsEmpty() {
		return "Двусвязный список пустой"
	}
	groups[groupName].DeleteStudentFromEnd()
	return "Элемент удален"
}

func LGET(groupName string) string {
	if _, exists := groups[groupName]; !exists {
		return "Двусвязный список не найден"
	}
	if groups[groupName].IsEmpty() {
		return "Двусвязный список пустой"
	}

	var output strings.Builder
	oldStdout := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	groups[groupName].PrintForward()

	w.Close()
	os.Stdout = oldStdout

	buf := make([]byte, 1024)
	n, _ := r.Read(buf)
	output.Write(buf[:n])

	result := output.String()
	if len(result) > 0 && result[len(result)-1] == '\n' {
		result = result[:len(result)-1]
	}
	return result
}

func LGETREV(groupName string) string {
	if _, exists := groups[groupName]; !exists {
		return "Двусвязный список не найден"
	}
	if groups[groupName].IsEmpty() {
		return "Двусвязный список пустой"
	}

	var output strings.Builder
	oldStdout := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	groups[groupName].PrintReverse()

	w.Close()
	os.Stdout = oldStdout

	buf := make([]byte, 1024)
	n, _ := r.Read(buf)
	output.Write(buf[:n])

	result := output.String()
	if len(result) > 0 && result[len(result)-1] == '\n' {
		result = result[:len(result)-1]
	}
	return result
}

// Функции для работы с массивами
func MCREATE(A string, C int) string {
	if _, exists := arrays[A]; exists {
		arrays[A] = NewArray(C)
	} else {
		arrays[A] = NewArray(C)
	}
	return strconv.Itoa(C)
}

func MPUSHIND(A string, index int, value string) string {
	if _, exists := arrays[A]; !exists {
		return "Массив не найден"
	}
	if index < 0 || index > arrays[A].RazmerMas() {
		return "Индекс вышел за границы"
	}
	if arrays[A].RazmerMas() >= arrays[A].GetCapacity() {
		return "Массив переполнен"
	}

	err := arrays[A].AddMasAtInd(index, value)
	if err != nil {
		return err.Error()
	}
	return value
}

func MPUSHEND(A, value string) string {
	if _, exists := arrays[A]; !exists {
		return "Массив не найден"
	}
	if arrays[A].RazmerMas() >= arrays[A].GetCapacity() {
		return "Массив переполнен"
	}

	err := arrays[A].AddMasAtEnd(value)
	if err != nil {
		return err.Error()
	}
	return value
}

func MGETIND(A string, index int) string {
	if _, exists := arrays[A]; !exists {
		return "Массив не найден"
	}
	if index < 0 || index >= arrays[A].RazmerMas() {
		return "Индекс вышел за границы"
	}
	value, err := arrays[A].PoiskMasPoInd(index)
	if err != nil {
		return err.Error()
	}
	return value
}

func MDELIND(A string, index int) string {
	if _, exists := arrays[A]; !exists {
		return "Массив не найден"
	}
	if index < 0 || index >= arrays[A].RazmerMas() {
		return "Индекс вышел за границы"
	}
	err := arrays[A].DeleteMasPoInd(index)
	if err != nil {
		return err.Error()
	}
	return "Элемент удален"
}

func MREPLACE(A string, index int, value string) string {
	if _, exists := arrays[A]; !exists {
		return "Массив не найден"
	}
	if index < 0 || index >= arrays[A].RazmerMas() {
		return "Индекс за границами"
	}
	err := arrays[A].ZamenaMas(index, value)
	if err != nil {
		return err.Error()
	}
	return value
}

func MSIZE(A string) string {
	if _, exists := arrays[A]; !exists {
		return "Массив не найден"
	}
	return strconv.Itoa(arrays[A].RazmerMas())
}

func MGET(A string) string {
	if _, exists := arrays[A]; !exists {
		return "Массив не найден"
	}
	if arrays[A].RazmerMas() == 0 {
		return "Массив пуст"
	}

	var output strings.Builder
	oldStdout := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	arrays[A].ReadMas()

	w.Close()
	os.Stdout = oldStdout

	buf := make([]byte, 1024)
	n, _ := r.Read(buf)
	output.Write(buf[:n])

	result := output.String()
	if len(result) > 0 && result[len(result)-1] == '\n' {
		result = result[:len(result)-1]
	}
	return result
}

// Функции для работы с односвязными списками
func FPUSHBEFORE(listName, targetElement, newElement string) string {
	if _, exists := lists[listName]; !exists {
		return "Список не найден"
	}
	target := lists[listName].FindElement(lists[listName], targetElement)
	if target == nil {
		return "Элемент не найден"
	}
	listPtr := lists[listName]
	lists[listName].AddChelDo(&listPtr, target, newElement)
	lists[listName] = listPtr
	return newElement
}

func FPUSHEND(listName, newElement string) string {
	if _, exists := lists[listName]; !exists {
		lists[listName] = &OdSpis{name: newElement, place: nil}
		return newElement
	}
	listPtr := lists[listName]
	lists[listName].AddToEnd(&listPtr, newElement)
	lists[listName] = listPtr
	return newElement
}

func FPUSHAFTER(listName, targetElement, newElement string) string {
	if _, exists := lists[listName]; !exists {
		return "Список не найден"
	}
	target := lists[listName].FindElement(lists[listName], targetElement)
	if target == nil {
		return "Элемент не найден"
	}
	lists[listName].AddChelPosle(target, newElement)
	return newElement
}

func FPUSHBEGIN(listName, newElement string) string {
	if _, exists := lists[listName]; !exists {
		lists[listName] = &OdSpis{name: newElement, place: nil}
		return newElement
	}
	listPtr := lists[listName]
	lists[listName].AddToBegin(&listPtr, newElement)
	lists[listName] = listPtr
	return newElement
}

func FPOISK(listName, searchElement string) string {
	if _, exists := lists[listName]; !exists {
		return "Список не найден"
	}
	found := lists[listName].Poisk(lists[listName], searchElement)
	if found {
		return "Найден"
	}
	return "НЕ найден"
}

func FDELVALUE(listName, elementToDelete string) string {
	if _, exists := lists[listName]; !exists {
		return "Список не найден"
	}
	if !lists[listName].Poisk(lists[listName], elementToDelete) {
		return "Элемент не найден"
	}
	listPtr := lists[listName]
	err := lists[listName].DeletePoZnach(&listPtr, elementToDelete)
	if err != nil {
		return err.Error()
	}
	lists[listName] = listPtr
	return "Удален"
}

func FDELBEGIN(listName string) string {
	if _, exists := lists[listName]; !exists {
		return "Список не найден"
	}
	if lists[listName] == nil {
		return "Список пуст"
	}
	listPtr := lists[listName]
	lists[listName].DeleteToBegin(&listPtr)
	lists[listName] = listPtr
	return "Удален"
}

func FDELEND(listName string) string {
	if _, exists := lists[listName]; !exists {
		return "Список не найден"
	}
	if lists[listName] == nil {
		return "Список пуст"
	}
	listPtr := lists[listName]
	lists[listName].DeleteToEnd(&listPtr)
	lists[listName] = listPtr
	return "Удален"
}

func FDELBEFORE(listName, targetElement string) string {
	if _, exists := lists[listName]; !exists {
		return "Список не найден"
	}
	target := lists[listName].FindElement(lists[listName], targetElement)
	if target == nil {
		return "Элемент не найден"
	}
	if target == lists[listName] {
		return "Не может быть удален несуществующий в пустоте элемент"
	}
	listPtr := lists[listName]
	lists[listName].DeleteChelDo(&listPtr, target)
	lists[listName] = listPtr
	return "Удален"
}

func FDELAFTER(listName, targetElement string) string {
	if _, exists := lists[listName]; !exists {
		return "Список не найден"
	}
	target := lists[listName].FindElement(lists[listName], targetElement)
	if target == nil {
		return "Элемент не найден"
	}
	if target.place == nil {
		return "Нельзя удалить элемент, существующий где-то после листа"
	}
	lists[listName].DeleteChelPosle(target)
	return "Удален"
}

func FGET(listName string) string {
	if _, exists := lists[listName]; !exists {
		return "Список не найден"
	}
	if lists[listName] == nil {
		return "Список пуст"
	}

	var output strings.Builder
	oldStdout := os.Stdout
	r, w, _ := os.Pipe()
	os.Stdout = w

	lists[listName].Print(lists[listName])

	w.Close()
	os.Stdout = oldStdout

	buf := make([]byte, 1024)
	n, _ := r.Read(buf)
	output.Write(buf[:n])

	result := output.String()
	if len(result) > 0 && result[len(result)-1] == '\n' {
		result = result[:len(result)-1]
	}
	return result
}

// Функции для работы со стеками и очередями
func processStackQueueCommand(command, structureName, item string, filename string) string {
	var result string

	if command == "SPUSH" || command == "QPUSH" {
		if item == "" {
			return "ОШИБКА: пустое значение"
		}

		if command == "SPUSH" {
			if _, exists := stacks[structureName]; !exists {
				stacks[structureName] = NewStack()
			}
			stacks[structureName].SPUSH(item)
		} else {
			if _, exists := queues[structureName]; !exists {
				queues[structureName] = NewQueue()
			}
			queues[structureName].QPUSH(item)
		}
		result = item
	} else if command == "SPOP" {
		if _, exists := stacks[structureName]; !exists || stacks[structureName].IsEmpty() {
			return "Стек пустой"
		}
		result = stacks[structureName].SPOP()
	} else if command == "QPOP" {
		if _, exists := queues[structureName]; !exists || queues[structureName].CountQueue() == 0 {
			return "Очередь пуста"
		}
		value, err := queues[structureName].QPOP()
		if err != nil {
			return err.Error()
		}
		result = value
	}

	saveToFile(filename)
	return result
}

// [Остальные функции saveTreeBinary, loadTreeBinary, saveHashTableBinary, loadHashTableBinary, saveToFile, loadFromFile, processListCommand, processHashCommand, processArrayCommand, processFListCommand, processTreeCommand, processQuery остаются без изменений]
// ... [вставьте сюда все остальные функции из предыдущего кода] ...

// Функции бинарной сериализации (остаются без изменений)
func saveTreeBinary(root *Tree, file *os.File) error {
	if root == nil {
		marker := int32(-1)
		return binary.Write(file, binary.LittleEndian, marker)
	}

	value := int32(root.getValue())
	if err := binary.Write(file, binary.LittleEndian, value); err != nil {
		return err
	}

	color := byte(0)
	if root.getColor() == RED {
		color = 'r'
	} else {
		color = 'b'
	}
	if err := binary.Write(file, binary.LittleEndian, color); err != nil {
		return err
	}

	if err := saveTreeBinary(root.getLeft(), file); err != nil {
		return err
	}
	return saveTreeBinary(root.getRight(), file)
}

func loadTreeBinary(file *os.File) (*Tree, error) {
	var value int32
	if err := binary.Read(file, binary.LittleEndian, &value); err != nil {
		return nil, err
	}
	if value == -1 {
		return nil, nil
	}

	var color byte
	if err := binary.Read(file, binary.LittleEndian, &color); err != nil {
		return nil, err
	}

	node := &Tree{}
	node.value = int(value)
	node.color = RED
	if color == 'b' {
		node.color = BLACK
	}

	leftChild, err := loadTreeBinary(file)
	if err != nil {
		return nil, err
	}

	rightChild, err := loadTreeBinary(file)
	if err != nil {
		return nil, err
	}

	node.left = leftChild
	node.right = rightChild

	if leftChild != nil {
		leftChild.parent = node
	}
	if rightChild != nil {
		rightChild.parent = node
	}

	return node, nil
}

func saveHashTableBinary(table *DoubleHashTable, file *os.File) error {
	if table == nil {
		count := int32(0)
		return binary.Write(file, binary.LittleEndian, count)
	}

	count := int32(table.ElementCount())
	if err := binary.Write(file, binary.LittleEndian, count); err != nil {
		return err
	}

	if count > 0 {
		keys, values, actualCount := table.GetAllElements()

		for i := 0; i < actualCount; i++ {
			key := int32(keys[i])
			if err := binary.Write(file, binary.LittleEndian, key); err != nil {
				return err
			}

			valueLength := int32(len(values[i]))
			if err := binary.Write(file, binary.LittleEndian, valueLength); err != nil {
				return err
			}
			if _, err := file.Write([]byte(values[i])); err != nil {
				return err
			}
		}
	}

	return nil
}

func loadHashTableBinary(file *os.File) (*DoubleHashTable, error) {
	table := NewDoubleHashTable()

	var count int32
	if err := binary.Read(file, binary.LittleEndian, &count); err != nil {
		return nil, err
	}

	for i := 0; i < int(count); i++ {
		var key int32
		if err := binary.Read(file, binary.LittleEndian, &key); err != nil {
			return nil, err
		}

		var valueLength int32
		if err := binary.Read(file, binary.LittleEndian, &valueLength); err != nil {
			return nil, err
		}

		valueBytes := make([]byte, valueLength)
		if _, err := file.Read(valueBytes); err != nil {
			return nil, err
		}
		value := string(valueBytes)

		table.ADD(int(key), value)
	}

	return table, nil
}

// Бинарное сохранение в файл (полная версия)
func saveToFile(filename string) {
	file, err := os.Create(filename)
	if err != nil {
		fmt.Printf("Ошибка открытия файла: %v\n", err)
		return
	}
	defer file.Close()

	// Сохраняем массивы [M]
	sectionM := int32(0x4D)
	if err := binary.Write(file, binary.LittleEndian, sectionM); err != nil {
		return
	}

	arrayCount := int32(len(arrays))
	if err := binary.Write(file, binary.LittleEndian, arrayCount); err != nil {
		return
	}

	for name, array := range arrays {
		nameLength := int32(len(name))
		if err := binary.Write(file, binary.LittleEndian, nameLength); err != nil {
			return
		}
		if _, err := file.Write([]byte(name)); err != nil {
			return
		}

		capacity := int32(array.GetCapacity())
		size := int32(array.RazmerMas())
		if err := binary.Write(file, binary.LittleEndian, capacity); err != nil {
			return
		}
		if err := binary.Write(file, binary.LittleEndian, size); err != nil {
			return
		}

		for i := 0; i < array.RazmerMas(); i++ {
			element, _ := array.PoiskMasPoInd(i)
			elemLength := int32(len(element))
			if err := binary.Write(file, binary.LittleEndian, elemLength); err != nil {
				return
			}
			if _, err := file.Write([]byte(element)); err != nil {
				return
			}
		}
	}

	// Сохраняем стеки [S]
	sectionS := int32(0x53)
	if err := binary.Write(file, binary.LittleEndian, sectionS); err != nil {
		return
	}

	stackCount := int32(len(stacks))
	if err := binary.Write(file, binary.LittleEndian, stackCount); err != nil {
		return
	}

	for name, stack := range stacks {
		nameLength := int32(len(name))
		if err := binary.Write(file, binary.LittleEndian, nameLength); err != nil {
			return
		}
		if _, err := file.Write([]byte(name)); err != nil {
			return
		}

		var stackItems []string
		current := stack.head
		for current != nil {
			stackItems = append([]string{current.value}, stackItems...)
			current = current.next
		}

		itemCount := int32(len(stackItems))
		if err := binary.Write(file, binary.LittleEndian, itemCount); err != nil {
			return
		}

		for _, item := range stackItems {
			itemLength := int32(len(item))
			if err := binary.Write(file, binary.LittleEndian, itemLength); err != nil {
				return
			}
			if _, err := file.Write([]byte(item)); err != nil {
				return
			}
		}
	}

	// Сохраняем односвязные списки [F]
	sectionF := int32(0x46)
	if err := binary.Write(file, binary.LittleEndian, sectionF); err != nil {
		return
	}

	listCount := int32(len(lists))
	if err := binary.Write(file, binary.LittleEndian, listCount); err != nil {
		return
	}

	for name, list := range lists {
		nameLength := int32(len(name))
		if err := binary.Write(file, binary.LittleEndian, nameLength); err != nil {
			return
		}
		if _, err := file.Write([]byte(name)); err != nil {
			return
		}

		var listItems []string
		current := list
		for current != nil {
			listItems = append(listItems, current.name)
			current = current.place
		}

		itemCount := int32(len(listItems))
		if err := binary.Write(file, binary.LittleEndian, itemCount); err != nil {
			return
		}

		for _, item := range listItems {
			itemLength := int32(len(item))
			if err := binary.Write(file, binary.LittleEndian, itemLength); err != nil {
				return
			}
			if _, err := file.Write([]byte(item)); err != nil {
				return
			}
		}
	}

	// Сохраняем двусвязные списки [L]
	sectionL := int32(0x4C)
	if err := binary.Write(file, binary.LittleEndian, sectionL); err != nil {
		return
	}

	groupCount := int32(len(groups))
	if err := binary.Write(file, binary.LittleEndian, groupCount); err != nil {
		return
	}

	for name, group := range groups {
		nameLength := int32(len(name))
		if err := binary.Write(file, binary.LittleEndian, nameLength); err != nil {
			return
		}
		if _, err := file.Write([]byte(name)); err != nil {
			return
		}

		var groupItems []string
		if !group.IsEmpty() {
			current := group.head
			for current != nil {
				groupItems = append(groupItems, current.student)
				current = current.next
			}
		}

		itemCount := int32(len(groupItems))
		if err := binary.Write(file, binary.LittleEndian, itemCount); err != nil {
			return
		}

		for _, item := range groupItems {
			itemLength := int32(len(item))
			if err := binary.Write(file, binary.LittleEndian, itemLength); err != nil {
				return
			}
			if _, err := file.Write([]byte(item)); err != nil {
				return
			}
		}
	}

	// Сохраняем деревья [T]
	sectionT := int32(0x54)
	if err := binary.Write(file, binary.LittleEndian, sectionT); err != nil {
		return
	}

	treeCount := int32(len(trees))
	if err := binary.Write(file, binary.LittleEndian, treeCount); err != nil {
		return
	}

	for name, tree := range trees {
		nameLength := int32(len(name))
		if err := binary.Write(file, binary.LittleEndian, nameLength); err != nil {
			return
		}
		if _, err := file.Write([]byte(name)); err != nil {
			return
		}

		if err := saveTreeBinary(tree, file); err != nil {
			return
		}
	}

	// Сохраняем хэш-таблицы [H]
	sectionH := int32(0x48)
	if err := binary.Write(file, binary.LittleEndian, sectionH); err != nil {
		return
	}

	hashCount := int32(len(hashTables))
	if err := binary.Write(file, binary.LittleEndian, hashCount); err != nil {
		return
	}

	for name, table := range hashTables {
		nameLength := int32(len(name))
		if err := binary.Write(file, binary.LittleEndian, nameLength); err != nil {
			return
		}
		if _, err := file.Write([]byte(name)); err != nil {
			return
		}

		if err := saveHashTableBinary(table, file); err != nil {
			return
		}
	}
}

// Функция бинарной загрузки
func loadFromFile(filename string) {
	file, err := os.Open(filename)
	if err != nil {
		return
	}
	defer file.Close()

	fileInfo, err := file.Stat()
	if err != nil || fileInfo.Size() == 0 {
		return
	}

	arrays = make(map[string]*Array)
	queues = make(map[string]*Queue)
	stacks = make(map[string]*Stack)
	lists = make(map[string]*OdSpis)
	groups = make(map[string]*StudentGroup)
	trees = make(map[string]*Tree)
	hashTables = make(map[string]*DoubleHashTable)

	for {
		var sectionMarker int32
		err := binary.Read(file, binary.LittleEndian, &sectionMarker)
		if err != nil {
			break
		}

		switch sectionMarker {
		case 0x4D: // 'M' - массивы
			var arrayCount int32
			if err := binary.Read(file, binary.LittleEndian, &arrayCount); err != nil {
				return
			}

			for i := 0; i < int(arrayCount); i++ {
				var nameLength int32
				if err := binary.Read(file, binary.LittleEndian, &nameLength); err != nil {
					return
				}
				nameBytes := make([]byte, nameLength)
				if _, err := file.Read(nameBytes); err != nil {
					return
				}
				arrayName := string(nameBytes)

				var capacity, size int32
				if err := binary.Read(file, binary.LittleEndian, &capacity); err != nil {
					return
				}
				if err := binary.Read(file, binary.LittleEndian, &size); err != nil {
					return
				}

				newArray := NewArray(int(capacity))

				for j := 0; j < int(size); j++ {
					var elemLength int32
					if err := binary.Read(file, binary.LittleEndian, &elemLength); err != nil {
						return
					}
					elemBytes := make([]byte, elemLength)
					if _, err := file.Read(elemBytes); err != nil {
						return
					}
					element := string(elemBytes)
					newArray.AddMasAtEnd(element)
				}

				arrays[arrayName] = newArray
			}

		case 0x53: // 'S' - стеки
			var stackCount int32
			if err := binary.Read(file, binary.LittleEndian, &stackCount); err != nil {
				return
			}

			for i := 0; i < int(stackCount); i++ {
				var nameLength int32
				if err := binary.Read(file, binary.LittleEndian, &nameLength); err != nil {
					return
				}
				nameBytes := make([]byte, nameLength)
				if _, err := file.Read(nameBytes); err != nil {
					return
				}
				stackName := string(nameBytes)

				stacks[stackName] = NewStack()

				var itemCount int32
				if err := binary.Read(file, binary.LittleEndian, &itemCount); err != nil {
					return
				}

				var items []string
				for j := 0; j < int(itemCount); j++ {
					var itemLength int32
					if err := binary.Read(file, binary.LittleEndian, &itemLength); err != nil {
						return
					}
					itemBytes := make([]byte, itemLength)
					if _, err := file.Read(itemBytes); err != nil {
						return
					}
					item := string(itemBytes)
					items = append(items, item)
				}

				for _, item := range items {
					stacks[stackName].SPUSH(item)
				}
			}

		case 0x46: // 'F' - односвязные списки
			var listCount int32
			if err := binary.Read(file, binary.LittleEndian, &listCount); err != nil {
				return
			}

			for i := 0; i < int(listCount); i++ {
				var nameLength int32
				if err := binary.Read(file, binary.LittleEndian, &nameLength); err != nil {
					return
				}
				nameBytes := make([]byte, nameLength)
				if _, err := file.Read(nameBytes); err != nil {
					return
				}
				listName := string(nameBytes)

				lists[listName] = nil

				var itemCount int32
				if err := binary.Read(file, binary.LittleEndian, &itemCount); err != nil {
					return
				}

				for j := 0; j < int(itemCount); j++ {
					var itemLength int32
					if err := binary.Read(file, binary.LittleEndian, &itemLength); err != nil {
						return
					}
					itemBytes := make([]byte, itemLength)
					if _, err := file.Read(itemBytes); err != nil {
						return
					}
					item := string(itemBytes)

					if lists[listName] == nil {
						lists[listName] = &OdSpis{name: item, place: nil}
					} else {
						listPtr := lists[listName]
						lists[listName].AddToEnd(&listPtr, item)
						lists[listName] = listPtr
					}
				}
			}

		case 0x4C: // 'L' - двусвязные списки
			var groupCount int32
			if err := binary.Read(file, binary.LittleEndian, &groupCount); err != nil {
				return
			}

			for i := 0; i < int(groupCount); i++ {
				var nameLength int32
				if err := binary.Read(file, binary.LittleEndian, &nameLength); err != nil {
					return
				}
				nameBytes := make([]byte, nameLength)
				if _, err := file.Read(nameBytes); err != nil {
					return
				}
				groupName := string(nameBytes)

				newGroup := NewStudentGroup()

				var itemCount int32
				if err := binary.Read(file, binary.LittleEndian, &itemCount); err != nil {
					return
				}

				for j := 0; j < int(itemCount); j++ {
					var itemLength int32
					if err := binary.Read(file, binary.LittleEndian, &itemLength); err != nil {
						return
					}
					itemBytes := make([]byte, itemLength)
					if _, err := file.Read(itemBytes); err != nil {
						return
					}
					item := string(itemBytes)
					newGroup.AddStudentToEnd(item)
				}

				groups[groupName] = newGroup
			}

		case 0x54: // 'T' - деревья
			var treeCount int32
			if err := binary.Read(file, binary.LittleEndian, &treeCount); err != nil {
				return
			}

			for i := 0; i < int(treeCount); i++ {
				var nameLength int32
				if err := binary.Read(file, binary.LittleEndian, &nameLength); err != nil {
					return
				}
				nameBytes := make([]byte, nameLength)
				if _, err := file.Read(nameBytes); err != nil {
					return
				}
				treeName := string(nameBytes)

				newTree, err := loadTreeBinary(file)
				if err != nil {
					continue
				}
				trees[treeName] = newTree
			}

		case 0x48: // 'H' - хэш-таблицы
			var hashCount int32
			if err := binary.Read(file, binary.LittleEndian, &hashCount); err != nil {
				return
			}

			for i := 0; i < int(hashCount); i++ {
				var nameLength int32
				if err := binary.Read(file, binary.LittleEndian, &nameLength); err != nil {
					return
				}
				nameBytes := make([]byte, nameLength)
				if _, err := file.Read(nameBytes); err != nil {
					return
				}
				tableName := string(nameBytes)

				newTable, err := loadHashTableBinary(file)
				if err != nil {
					continue
				}
				hashTables[tableName] = newTable
			}

		default:
			return
		}
	}
}

// Функции обработки команд
func processListCommand(command string, args []string, filename string) string {
	if len(args) < 1 {
		return "GROUP_NOT_FOUND"
	}
	groupName := args[0]

	if _, exists := groups[groupName]; !exists &&
		command != "LPUSHBEGIN" && command != "LPUSHEND" {
		return "GROUP_NOT_FOUND"
	}

	var result string
	switch command {
	case "LPUSHBEGIN":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = LPUSHBEGIN(groupName, args[1])
	case "LPUSHEND":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = LPUSHEND(groupName, args[1])
	case "LPUSHBEFORE":
		if len(args) < 3 {
			return "ОШИБКА"
		}
		result = LPUSHBEFORE(groupName, args[1], args[2])
	case "LPUSHAFTER":
		if len(args) < 3 {
			return "ОШИБКА"
		}
		result = LPUSHAFTER(groupName, args[1], args[2])
	case "LPOPBEFORE":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = LPOPBEFORE(groupName, args[1])
	case "LPOPAFTER":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = LPOPAFTER(groupName, args[1])
	case "LPOPBEGIN":
		result = LPOPBEGIN(groupName)
	case "LPOPEND":
		result = LPOPEND(groupName)
	case "LPOISK":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = LPOISK(groupName, args[1])
	case "LPOPVALUE":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = LPOPVALUE(groupName, args[1])
	case "LGET":
		result = LGET(groupName)
	case "LGETREV":
		result = LGETREV(groupName)
	default:
		return "Неизвестная команда"
	}

	if result != "Двусвязный список не найден" && result != "Элемент не найден" &&
		result != "Ошибка" && result != "" {
		saveToFile(filename)
	}
	return result
}

func processHashCommand(command string, args []string, filename string) string {
	if len(args) < 1 {
		return "ОШИБКА"
	}
	tableName := args[0]

	var result string
	switch command {
	case "HCREATE":
		result = HCREATE(tableName)
	case "HADD":
		if len(args) < 3 {
			return "ОШИБКА"
		}
		key, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = HADD(tableName, key, args[2])
	case "HDEL":
		if len(args) < 3 {
			return "ОШИБКА"
		}
		key, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = HDEL(tableName, key, args[2])
	case "HSEARCH":
		if len(args) < 3 {
			return "ОШИБКА"
		}
		key, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = HSEARCH(tableName, key, args[2])
	case "HGET":
		result = HGET(tableName)
	case "HSIZE":
		result = HSIZE(tableName)
	default:
		return "Неизвестная команда"
	}

	if command != "HGET" && command != "HSIZE" && command != "HSEARCH" &&
		result != "Хэш-таблица не найдена" && result != "Элемент не найден" &&
		result != "ОШИБКА" && result != "" {
		saveToFile(filename)
	}

	return result
}

func processArrayCommand(command string, args []string, filename string) string {
	if len(args) < 1 {
		return "ОШИБКА"
	}
	arrayName := args[0]

	var result string
	switch command {
	case "MCREATE":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		capacity, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = MCREATE(arrayName, capacity)
	case "MPUSHIND":
		if len(args) < 3 {
			return "ОШИБКА"
		}
		index, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = MPUSHIND(arrayName, index, args[2])
	case "MPUSHEND":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = MPUSHEND(arrayName, args[1])
	case "MGETIND":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		index, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = MGETIND(arrayName, index)
	case "MDELIND":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		index, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = MDELIND(arrayName, index)
	case "MREPLACE":
		if len(args) < 3 {
			return "ОШИБКА"
		}
		index, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = MREPLACE(arrayName, index, args[2])
	case "MSIZE":
		result = MSIZE(arrayName)
	case "MGET":
		result = MGET(arrayName)
	default:
		return "Неизвестная команда"
	}

	if result != "Массив не найден" && result != "Индекс вышел за границы" &&
		result != "Индекс за границами" && result != "Массив переполнен" &&
		result != "ОШИБКА" {
		saveToFile(filename)
	}
	return result
}

func processFListCommand(command string, args []string, filename string) string {
	if len(args) < 1 {
		return "ОШИБКА"
	}
	listName := args[0]

	var result string
	switch command {
	case "FPUSHBEFORE", "FPUSHAFTER":
		if len(args) < 3 {
			return "ОШИБКА"
		}
		if command == "FPUSHBEFORE" {
			result = FPUSHBEFORE(listName, args[1], args[2])
		} else {
			result = FPUSHAFTER(listName, args[1], args[2])
		}
	case "FPUSHEND", "FPUSHBEGIN":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		if command == "FPUSHEND" {
			result = FPUSHEND(listName, args[1])
		} else {
			result = FPUSHBEGIN(listName, args[1])
		}
	case "FPOISK":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = FPOISK(listName, args[1])
	case "FDELVALUE":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = FDELVALUE(listName, args[1])
	case "FDELBEGIN":
		result = FDELBEGIN(listName)
	case "FDELEND":
		result = FDELEND(listName)
	case "FDELBEFORE":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = FDELBEFORE(listName, args[1])
	case "FDELAFTER":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		result = FDELAFTER(listName, args[1])
	case "FGET":
		return FGET(listName)
	default:
		return "Неизвестная команда"
	}

	if result != "Список не найден" && result != "Элемент не найден" && result != "Список пуст" {
		saveToFile(filename)
	}
	return result
}

func processTreeCommand(command string, args []string, filename string) string {
	if len(args) < 1 {
		return "ОШИБКА"
	}
	treeName := args[0]

	var result string
	switch command {
	case "TADD":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		value, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = TADD(treeName, value)
	case "TDEL":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		value, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = TDEL(treeName, value)
	case "TSEARCH":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		value, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = TSEARCH(treeName, value)
	case "TDELETE":
		if len(args) < 2 {
			return "ОШИБКА"
		}
		value, err := strconv.Atoi(args[1])
		if err != nil {
			return "ОШИБКА"
		}
		result = TDELETE(treeName, value)
	case "TGET":
		result = TGET(treeName)
	case "TGETSYM":
		result = TGETSYM(treeName)
	case "TREAD":
		result = TREAD(treeName)
	default:
		return "Неизвестная команда"
	}

	if command != "TREAD" && command != "TGET" && command != "TGETSYM" &&
		command != "TSEARCH" && result != "Дерево не найдено" &&
		result != "Элемент не найден или невозможно удалить" &&
		result != "ОШИБКА" && result != "" {
		saveToFile(filename)
	}
	return result
}

func processQuery(query, filename string) string {
	parts := strings.Fields(query)
	if len(parts) == 0 {
		return "Пустая команда"
	}

	command := parts[0]
	args := parts[1:]

	if strings.HasPrefix(command, "L") {
		return processListCommand(command, args, filename)
	} else if strings.HasPrefix(command, "M") {
		return processArrayCommand(command, args, filename)
	} else if strings.HasPrefix(command, "F") {
		return processFListCommand(command, args, filename)
	} else if command == "SPUSH" || command == "SPOP" || command == "QPUSH" || command == "QPOP" {
		if len(args) < 1 {
			return "ОШИБКА: недостаточно аргументов"
		}
		structureName := args[0]
		item := ""
		if len(args) > 1 {
			item = strings.Join(args[1:], " ")
		}
		return processStackQueueCommand(command, structureName, item, filename)
	} else if strings.HasPrefix(command, "T") {
		return processTreeCommand(command, args, filename)
	} else if strings.HasPrefix(command, "H") {
		return processHashCommand(command, args, filename)
	}
	return "Неизвестная команда"
}
