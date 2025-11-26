package main

import (
	"encoding/json"
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

// Структуры для JSON сериализации
type JSONData struct {
	Arrays      map[string]JSONArray       `json:"arrays"`
	Queues      map[string][]string        `json:"queues"`
	Stacks      map[string][]string        `json:"stacks"`
	SingleLists map[string][]string        `json:"single_lists"`
	DoubleLists map[string][]string        `json:"double_lists"`
	Trees       map[string]JSONTreeNode    `json:"trees"`
	HashTables  map[string][]JSONHashEntry `json:"hash_tables"`
}

type JSONArray struct {
	Capacity int      `json:"capacity"`
	Size     int      `json:"size"`
	Elements []string `json:"elements"`
}

type JSONTreeNode struct {
	Value int            `json:"value"`
	Color string         `json:"color"`
	Left  *JSONTreeNode  `json:"left,omitempty"`
	Right *JSONTreeNode  `json:"right,omitempty"`
}

type JSONHashEntry struct {
	Key   int    `json:"key"`
	Value string `json:"value"`
}

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

// Вспомогательные функции для JSON сериализации деревьев
func treeToJSON(node *Tree) *JSONTreeNode {
	if node == nil {
		return nil
	}

	jsonNode := &JSONTreeNode{
		Value: node.getValue(),
		Color: "r",
	}

	if node.getColor() == BLACK {
		jsonNode.Color = "b"
	}

	if node.getLeft() != nil {
		jsonNode.Left = treeToJSON(node.getLeft())
	}

	if node.getRight() != nil {
		jsonNode.Right = treeToJSON(node.getRight())
	}

	return jsonNode
}

func treeFromJSON(jsonNode *JSONTreeNode) *Tree {
	if jsonNode == nil {
		return nil
	}

	node := &Tree{}
	node.value = jsonNode.Value
	node.color = RED
	if jsonNode.Color == "b" {
		node.color = BLACK
	}

	if jsonNode.Left != nil {
		node.left = treeFromJSON(jsonNode.Left)
		if node.left != nil {
			node.left.parent = node
		}
	}

	if jsonNode.Right != nil {
		node.right = treeFromJSON(jsonNode.Right)
		if node.right != nil {
			node.right.parent = node
		}
	}

	return node
}

// Вспомогательные функции для JSON сериализации хэш-таблиц
func hashTableToJSON(table *DoubleHashTable) []JSONHashEntry {
	if table == nil {
		return []JSONHashEntry{}
	}

	keys, values, count := table.GetAllElements()
	entries := make([]JSONHashEntry, count)

	for i := 0; i < count; i++ {
		entries[i] = JSONHashEntry{
			Key:   keys[i],
			Value: values[i],
		}
	}

	return entries
}

func hashTableFromJSON(entries []JSONHashEntry) *DoubleHashTable {
	table := NewDoubleHashTable()

	for _, entry := range entries {
		table.ADD(entry.Key, entry.Value)
	}

	return table
}

// Основные функции JSON сериализации/десериализации
func saveToFile(filename string) {
	data := JSONData{
		Arrays:      make(map[string]JSONArray),
		Queues:      make(map[string][]string),
		Stacks:      make(map[string][]string),
		SingleLists: make(map[string][]string),
		DoubleLists: make(map[string][]string),
		Trees:       make(map[string]JSONTreeNode),
		HashTables:  make(map[string][]JSONHashEntry),
	}

	// Сохраняем массивы
	for name, array := range arrays {
		elements := make([]string, array.RazmerMas())
		for i := 0; i < array.RazmerMas(); i++ {
			element, _ := array.PoiskMasPoInd(i)
			elements[i] = element
		}

		data.Arrays[name] = JSONArray{
			Capacity: array.GetCapacity(),
			Size:     array.RazmerMas(),
			Elements: elements,
		}
	}

	// Сохраняем очереди
	for name, queue := range queues {
		var items []string
		current := queue.head
		for current != nil {
			items = append(items, current.toy)
			current = current.next
		}
		data.Queues[name] = items
	}

	// Сохраняем стеки
	for name, stack := range stacks {
		var items []string
		current := stack.head
		for current != nil {
			items = append(items, current.value)
			current = current.next
		}
		data.Stacks[name] = items
	}

	// Сохраняем односвязные списки
	for name, list := range lists {
		var items []string
		current := list
		for current != nil {
			items = append(items, current.name)
			current = current.place
		}
		data.SingleLists[name] = items
	}

	// Сохраняем двусвязные списки
	for name, group := range groups {
		var items []string
		current := group.head
		for current != nil {
			items = append(items, current.student)
			current = current.next
		}
		data.DoubleLists[name] = items
	}

	// Сохраняем деревья
	for name, tree := range trees {
		if tree != nil {
			jsonTree := treeToJSON(tree)
			if jsonTree != nil {
				data.Trees[name] = *jsonTree
			}
		}
	}

	// Сохраняем хэш-таблицы
	for name, table := range hashTables {
		data.HashTables[name] = hashTableToJSON(table)
	}

	// Записываем JSON в файл
	file, err := os.Create(filename)
	if err != nil {
		fmt.Printf("Ошибка открытия файла: %v\n", err)
		return
	}
	defer file.Close()

	encoder := json.NewEncoder(file)
	encoder.SetIndent("", "  ")
	if err := encoder.Encode(data); err != nil {
		fmt.Printf("Ошибка кодирования JSON: %v\n", err)
		return
	}
}

func loadFromFile(filename string) {
	file, err := os.Open(filename)
	if err != nil {
		// Если файл не существует, просто выходим - это нормально при первом запуске
		return
	}
	defer file.Close()

	// Проверяем, не пустой ли файл
	fileInfo, err := file.Stat()
	if err != nil || fileInfo.Size() == 0 {
		return
	}

	var data JSONData
	decoder := json.NewDecoder(file)
	if err := decoder.Decode(&data); err != nil {
		fmt.Printf("Ошибка парсинга JSON: %v\n", err)
		return
	}

	// Очищаем существующие структуры
	arrays = make(map[string]*Array)
	queues = make(map[string]*Queue)
	stacks = make(map[string]*Stack)
	lists = make(map[string]*OdSpis)
	groups = make(map[string]*StudentGroup)
	trees = make(map[string]*Tree)
	hashTables = make(map[string]*DoubleHashTable)

	// Загружаем массивы
	for name, arrayData := range data.Arrays {
		newArray := NewArray(arrayData.Capacity)
		for _, element := range arrayData.Elements {
			newArray.AddMasAtEnd(element)
		}
		arrays[name] = newArray
	}

	// Загружаем очереди
	for name, queueData := range data.Queues {
		queues[name] = NewQueue()
		for _, item := range queueData {
			queues[name].QPUSH(item)
		}
	}

	// Загружаем стеки
	for name, stackData := range data.Stacks {
		stacks[name] = NewStack()
		for i := len(stackData) - 1; i >= 0; i-- {
			stacks[name].SPUSH(stackData[i])
		}
	}

	// Загружаем односвязные списки
	for name, listData := range data.SingleLists {
		if len(listData) > 0 {
			lists[name] = &OdSpis{name: listData[0]}
			current := lists[name]
			for i := 1; i < len(listData); i++ {
				current.place = &OdSpis{name: listData[i]}
				current = current.place
			}
		}
	}

	// Загружаем двусвязные списки
	for name, groupData := range data.DoubleLists {
		if len(groupData) > 0 {
			groups[name] = NewStudentGroup()
			for _, item := range groupData {
				groups[name].AddStudentToEnd(item)
			}
		}
	}

	// Загружаем деревья
	for name, treeData := range data.Trees {
		trees[name] = treeFromJSON(&treeData)
	}

	// Загружаем хэш-таблицы
	for name, hashData := range data.HashTables {
		hashTables[name] = hashTableFromJSON(hashData)
	}
}

// Функции для работы с массивами
func MCREATE(arrayName string, capacity int) string {
	arrays[arrayName] = NewArray(capacity)
	return "Массив " + arrayName + " создан с емкостью " + strconv.Itoa(capacity)
}

func MPUSHEND(arrayName string, value string) string {
	if array, exists := arrays[arrayName]; exists {
		err := array.AddMasAtEnd(value)
		if err != nil {
			return "Ошибка добавления: " + err.Error()
		}
		return value
	}
	return "Массив не найден"
}

func MPUSHIND(arrayName string, index int, value string) string {
	if array, exists := arrays[arrayName]; exists {
		err := array.AddMasAtInd(index, value)
		if err != nil {
			return "Ошибка добавления: " + err.Error()
		}
		return value
	}
	return "Массив не найден"
}

func MGETIND(arrayName string, index int) string {
	if array, exists := arrays[arrayName]; exists {
		value, err := array.PoiskMasPoInd(index)
		if err != nil {
			return "Ошибка: " + err.Error()
		}
		return value
	}
	return "Массив не найден"
}

func MDELIND(arrayName string, index int) string {
	if array, exists := arrays[arrayName]; exists {
		err := array.DeleteMasPoInd(index)
		if err != nil {
			return "Ошибка удаления: " + err.Error()
		}
		return "Элемент удален"
	}
	return "Массив не найден"
}

func MREPLACE(arrayName string, index int, value string) string {
	if array, exists := arrays[arrayName]; exists {
		err := array.ZamenaMas(index, value)
		if err != nil {
			return "Ошибка замены: " + err.Error()
		}
		return value
	}
	return "Массив не найден"
}

func MSIZE(arrayName string) string {
	if array, exists := arrays[arrayName]; exists {
		return strconv.Itoa(array.RazmerMas())
	}
	return "Массив не найден"
}

func MPRINT(arrayName string) string {
	if array, exists := arrays[arrayName]; exists {
		var result strings.Builder
		for i := 0; i < array.RazmerMas(); i++ {
			element, _ := array.PoiskMasPoInd(i)
			result.WriteString(element)
			if i < array.RazmerMas()-1 {
				result.WriteString(" ")
			}
		}
		return result.String()
	}
	return "Массив не найден"
}

// Функции для работы с очередями
func QCREATE(queueName string) string {
	queues[queueName] = NewQueue()
	return "Очередь " + queueName + " создана"
}

func QPUSH(queueName string, value string) string {
	if _, exists := queues[queueName]; !exists {
		queues[queueName] = NewQueue()
	}
	queues[queueName].QPUSH(value)
	return value
}

func QPOP(queueName string) string {
	if queue, exists := queues[queueName]; exists {
		value, err := queue.QPOP()
		if err != nil {
			return "Очередь пуста"
		}
		return value
	}
	return "Очередь не найдена"
}

func QPRINT(queueName string) string {
	if queue, exists := queues[queueName]; exists {
		var result strings.Builder
		current := queue.head
		for current != nil {
			result.WriteString(current.toy)
			if current.next != nil {
				result.WriteString(" ")
			}
			current = current.next
		}
		if result.Len() == 0 {
			return "Очередь пуста"
		}
		return result.String()
	}
	return "Очередь не найдена"
}

// Функции для работы со стеками
func SCREATE(stackName string) string {
	stacks[stackName] = NewStack()
	return "Стек " + stackName + " создан"
}

func SPUSH(stackName string, value string) string {
	if _, exists := stacks[stackName]; !exists {
		stacks[stackName] = NewStack()
	}
	stacks[stackName].SPUSH(value)
	return value
}

func SPOP(stackName string) string {
	if stack, exists := stacks[stackName]; exists {
		return stack.SPOP()
	}
	return "Стек не найден"
}

func SPRINT(stackName string) string {
	if stack, exists := stacks[stackName]; exists {
		var result strings.Builder
		current := stack.head
		for current != nil {
			result.WriteString(current.value)
			if current.next != nil {
				result.WriteString(" ")
			}
			current = current.next
		}
		if result.Len() == 0 {
			return "Стек пуст"
		}
		return result.String()
	}
	return "Стек не найден"
}

// Функции для работы с односвязными списками (F-команды)
func FCREATE(listName string) string {
	lists[listName] = nil
	return "Список " + listName + " создан"
}

func FPUSHBEGIN(listName string, value string) string {
	if _, exists := lists[listName]; !exists {
		lists[listName] = &OdSpis{name: value}
	} else {
		newElement := &OdSpis{name: value, place: lists[listName]}
		lists[listName] = newElement
	}
	return value
}

func FPUSHEND(listName string, value string) string {
	if _, exists := lists[listName]; !exists {
		lists[listName] = &OdSpis{name: value}
		return value
	}
	current := lists[listName]
	for current.place != nil {
		current = current.place
	}
	current.place = &OdSpis{name: value}
	return value
}

func FPUSHBEFORE(listName string, target string, value string) string {
	if list, exists := lists[listName]; exists {
		targetNode := list.FindElement(list, target)
		if targetNode == nil {
			return "Целевой элемент не найден"
		}
		list.AddChelDo(&list, targetNode, value)
		lists[listName] = list
		return value
	}
	return "Список не найден"
}

func FPUSHAFTER(listName string, target string, value string) string {
	if list, exists := lists[listName]; exists {
		targetNode := list.FindElement(list, target)
		if targetNode == nil {
			return "Целевой элемент не найден"
		}
		list.AddChelPosle(targetNode, value)
		return value
	}
	return "Список не найден"
}

func FPOISK(listName string, value string) string {
	if list, exists := lists[listName]; exists {
		found := list.Poisk(list, value)
		if found {
			return "Найден"
		}
		return "Не найден"
	}
	return "Список не найден"
}

func FDELVALUE(listName string, value string) string {
	if list, exists := lists[listName]; exists {
		err := list.DeletePoZnach(&list, value)
		if err != nil {
			return "Элемент не найден"
		}
		lists[listName] = list
		return "Удален"
	}
	return "Список не найден"
}

func FDELBEGIN(listName string) string {
	if list, exists := lists[listName]; exists {
		if list == nil {
			return "Список пуст"
		}
		list.DeleteToBegin(&list)
		lists[listName] = list
		return "Удален"
	}
	return "Список не найден"
}

func FDELEND(listName string) string {
	if list, exists := lists[listName]; exists {
		if list == nil {
			return "Список пуст"
		}
		list.DeleteToEnd(&list)
		return "Удален"
	}
	return "Список не найден"
}

func FDELBEFORE(listName string, target string) string {
	if list, exists := lists[listName]; exists {
		targetNode := list.FindElement(list, target)
		if targetNode == nil {
			return "Целевой элемент не найден"
		}
		list.DeleteChelDo(&list, targetNode)
		lists[listName] = list
		return "Удален"
	}
	return "Список не найден"
}

func FDELAFTER(listName string, target string) string {
	if list, exists := lists[listName]; exists {
		targetNode := list.FindElement(list, target)
		if targetNode == nil {
			return "Целевой элемент не найден"
		}
		list.DeleteChelPosle(targetNode)
		return "Удален"
	}
	return "Список не найден"
}

func FGET(listName string) string {
	if list, exists := lists[listName]; exists {
		var result strings.Builder
		current := list
		for current != nil {
			result.WriteString(current.name)
			if current.place != nil {
				result.WriteString(" ")
			}
			current = current.place
		}
		if result.Len() == 0 {
			return "Список пуст"
		}
		return result.String()
	}
	return "Список не найден"
}

// Функции для работы с двусвязными списками (L-команды)
func LCREATE(groupName string) string {
	groups[groupName] = NewStudentGroup()
	return "Группа " + groupName + " создана"
}

func LPUSHBEGIN(groupName string, student string) string {
	if _, exists := groups[groupName]; !exists {
		groups[groupName] = NewStudentGroup()
	}
	groups[groupName].AddStudentToStart(student)
	return student
}

func LPUSHEND(groupName string, student string) string {
	if _, exists := groups[groupName]; !exists {
		groups[groupName] = NewStudentGroup()
	}
	groups[groupName].AddStudentToEnd(student)
	return student
}

func LPUSHBEFORE(groupName string, target string, student string) string {
	if group, exists := groups[groupName]; exists {
		group.AddStudentBefore(target, student)
		return student
	}
	return "Группа не найдена"
}

func LPUSHAFTER(groupName string, target string, student string) string {
	if group, exists := groups[groupName]; exists {
		group.AddStudentAfter(target, student)
		return student
	}
	return "Группа не найдена"
}

func LPOPBEGIN(groupName string) string {
	if group, exists := groups[groupName]; exists {
		if group.head == nil {
			return "Группа пуста"
		}
		value := group.head.student
		group.DeleteStudentFromStart()
		return value
	}
	return "Группа не найдена"
}

func LPOPEND(groupName string) string {
	if group, exists := groups[groupName]; exists {
		if group.head == nil {
			return "Группа пуста"
		}
		value := group.tail.student
		group.DeleteStudentFromEnd()
		return value
	}
	return "Группа не найдена"
}

func LPOPBEFORE(groupName string, target string) string {
	if group, exists := groups[groupName]; exists {
		group.DeleteStudentBefore(target)
		return "Элемент удален"
	}
	return "Группа не найдена"
}

func LPOPAFTER(groupName string, target string) string {
	if group, exists := groups[groupName]; exists {
		group.DeleteStudentAfter(target)
		return "Элемент удален"
	}
	return "Группа не найдена"
}

func LPOPVALUE(groupName string, student string) string {
	if group, exists := groups[groupName]; exists {
		err := group.DeleteStudentByValue(student)
		if err != nil {
			return "Элемент не найден"
		}
		return student
	}
	return "Группа не найдена"
}

func LPOISK(groupName string, student string) string {
	if group, exists := groups[groupName]; exists {
		if group.Contains(student) {
			return "Найден"
		}
		return "Не найден"
	}
	return "Группа не найдена"
}

func LGET(groupName string) string {
	if group, exists := groups[groupName]; exists {
		var result strings.Builder
		current := group.head
		for current != nil {
			result.WriteString(current.student)
			if current.next != nil {
				result.WriteString(" ")
			}
			current = current.next
		}
		if result.Len() == 0 {
			return "Группа пуста"
		}
		return result.String()
	}
	return "Группа не найдена"
}

func LGETREV(groupName string) string {
	if group, exists := groups[groupName]; exists {
		var result strings.Builder
		current := group.tail
		for current != nil {
			result.WriteString(current.student)
			if current.prev != nil {
				result.WriteString(" ")
			}
			current = current.prev
		}
		if result.Len() == 0 {
			return "Группа пуста"
		}
		return result.String()
	}
	return "Группа не найдена"
}

// Функция обработки запросов
func processQuery(query string) string {
	parts := strings.Fields(query)
	if len(parts) == 0 {
		return "Пустая команда"
	}

	command := parts[0]
	
	switch command {
	// Массивы (M-команды)
	case "MCREATE":
		if len(parts) < 3 {
			return "Недостаточно параметров для MCREATE"
		}
		capacity, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверная емкость"
		}
		return MCREATE(parts[1], capacity)
	case "MPUSHEND":
		if len(parts) < 3 {
			return "Недостаточно параметров для MPUSHEND"
		}
		return MPUSHEND(parts[1], strings.Join(parts[2:], " "))
	case "MPUSHIND":
		if len(parts) < 4 {
			return "Недостаточно параметров для MPUSHIND"
		}
		index, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверный индекс"
		}
		return MPUSHIND(parts[1], index, strings.Join(parts[3:], " "))
	case "MGETIND":
		if len(parts) < 3 {
			return "Недостаточно параметров для MGETIND"
		}
		index, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверный индекс"
		}
		return MGETIND(parts[1], index)
	case "MDELIND":
		if len(parts) < 3 {
			return "Недостаточно параметров для MDELIND"
		}
		index, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверный индекс"
		}
		return MDELIND(parts[1], index)
	case "MREPLACE":
		if len(parts) < 4 {
			return "Недостаточно параметров для MREPLACE"
		}
		index, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверный индекс"
		}
		return MREPLACE(parts[1], index, strings.Join(parts[3:], " "))
	case "MSIZE":
		if len(parts) < 2 {
			return "Недостаточно параметров для MSIZE"
		}
		return MSIZE(parts[1])
	case "MPRINT":
		if len(parts) < 2 {
			return "Недостаточно параметров для MPRINT"
		}
		return MPRINT(parts[1])
		
	// Очереди (Q-команды)
	case "QCREATE":
		if len(parts) < 2 {
			return "Недостаточно параметров для QCREATE"
		}
		return QCREATE(parts[1])
	case "QPUSH":
		if len(parts) < 3 {
			return "Недостаточно параметров для QPUSH"
		}
		return QPUSH(parts[1], strings.Join(parts[2:], " "))
	case "QPOP":
		if len(parts) < 2 {
			return "Недостаточно параметров для QPOP"
		}
		return QPOP(parts[1])
	case "QPRINT":
		if len(parts) < 2 {
			return "Недостаточно параметров для QPRINT"
		}
		return QPRINT(parts[1])
		
	// Стеки (S-команды)
	case "SCREATE":
		if len(parts) < 2 {
			return "Недостаточно параметров для SCREATE"
		}
		return SCREATE(parts[1])
	case "SPUSH":
		if len(parts) < 3 {
			return "Недостаточно параметров для SPUSH"
		}
		return SPUSH(parts[1], strings.Join(parts[2:], " "))
	case "SPOP":
		if len(parts) < 2 {
			return "Недостаточно параметров для SPOP"
		}
		return SPOP(parts[1])
	case "SPRINT":
		if len(parts) < 2 {
			return "Недостаточно параметров для SPRINT"
		}
		return SPRINT(parts[1])
		
	// Односвязные списки (F-команды)
	case "FCREATE":
		if len(parts) < 2 {
			return "Недостаточно параметров для FCREATE"
		}
		return FCREATE(parts[1])
	case "FPUSHBEGIN":
		if len(parts) < 3 {
			return "Недостаточно параметров для FPUSHBEGIN"
		}
		return FPUSHBEGIN(parts[1], strings.Join(parts[2:], " "))
	case "FPUSHEND":
		if len(parts) < 3 {
			return "Недостаточно параметров для FPUSHEND"
		}
		return FPUSHEND(parts[1], strings.Join(parts[2:], " "))
	case "FPUSHBEFORE":
		if len(parts) < 4 {
			return "Недостаточно параметров для FPUSHBEFORE"
		}
		return FPUSHBEFORE(parts[1], parts[2], strings.Join(parts[3:], " "))
	case "FPUSHAFTER":
		if len(parts) < 4 {
			return "Недостаточно параметров для FPUSHAFTER"
		}
		return FPUSHAFTER(parts[1], parts[2], strings.Join(parts[3:], " "))
	case "FPOISK":
		if len(parts) < 3 {
			return "Недостаточно параметров для FPOISK"
		}
		return FPOISK(parts[1], strings.Join(parts[2:], " "))
	case "FDELVALUE":
		if len(parts) < 3 {
			return "Недостаточно параметров для FDELVALUE"
		}
		return FDELVALUE(parts[1], strings.Join(parts[2:], " "))
	case "FDELBEGIN":
		if len(parts) < 2 {
			return "Недостаточно параметров для FDELBEGIN"
		}
		return FDELBEGIN(parts[1])
	case "FDELEND":
		if len(parts) < 2 {
			return "Недостаточно параметров для FDELEND"
		}
		return FDELEND(parts[1])
	case "FDELBEFORE":
		if len(parts) < 3 {
			return "Недостаточно параметров для FDELBEFORE"
		}
		return FDELBEFORE(parts[1], parts[2])
	case "FDELAFTER":
		if len(parts) < 3 {
			return "Недостаточно параметров для FDELAFTER"
		}
		return FDELAFTER(parts[1], parts[2])
	case "FGET":
		if len(parts) < 2 {
			return "Недостаточно параметров для FGET"
		}
		return FGET(parts[1])
		
	// Двусвязные списки (L-команды)
	case "LCREATE":
		if len(parts) < 2 {
			return "Недостаточно параметров для LCREATE"
		}
		return LCREATE(parts[1])
	case "LPUSHBEGIN":
		if len(parts) < 3 {
			return "Недостаточно параметров для LPUSHBEGIN"
		}
		return LPUSHBEGIN(parts[1], strings.Join(parts[2:], " "))
	case "LPUSHEND":
		if len(parts) < 3 {
			return "Недостаточно параметров для LPUSHEND"
		}
		return LPUSHEND(parts[1], strings.Join(parts[2:], " "))
	case "LPUSHBEFORE":
		if len(parts) < 4 {
			return "Недостаточно параметров для LPUSHBEFORE"
		}
		return LPUSHBEFORE(parts[1], parts[2], strings.Join(parts[3:], " "))
	case "LPUSHAFTER":
		if len(parts) < 4 {
			return "Недостаточно параметров для LPUSHAFTER"
		}
		return LPUSHAFTER(parts[1], parts[2], strings.Join(parts[3:], " "))
	case "LPOPBEGIN":
		if len(parts) < 2 {
			return "Недостаточно параметров для LPOPBEGIN"
		}
		return LPOPBEGIN(parts[1])
	case "LPOPEND":
		if len(parts) < 2 {
			return "Недостаточно параметров для LPOPEND"
		}
		return LPOPEND(parts[1])
	case "LPOPBEFORE":
		if len(parts) < 3 {
			return "Недостаточно параметров для LPOPBEFORE"
		}
		return LPOPBEFORE(parts[1], parts[2])
	case "LPOPAFTER":
		if len(parts) < 3 {
			return "Недостаточно параметров для LPOPAFTER"
		}
		return LPOPAFTER(parts[1], parts[2])
	case "LPOPVALUE":
		if len(parts) < 3 {
			return "Недостаточно параметров для LPOPVALUE"
		}
		return LPOPVALUE(parts[1], strings.Join(parts[2:], " "))
	case "LPOISK":
		if len(parts) < 3 {
			return "Недостаточно параметров для LPOISK"
		}
		return LPOISK(parts[1], strings.Join(parts[2:], " "))
	case "LGET":
		if len(parts) < 2 {
			return "Недостаточно параметров для LGET"
		}
		return LGET(parts[1])
	case "LGETREV":
		if len(parts) < 2 {
			return "Недостаточно параметров для LGETREV"
		}
		return LGETREV(parts[1])
		
	// Деревья (T-команды)
	case "TADD":
		if len(parts) < 3 {
			return "Недостаточно параметров для TADD"
		}
		value, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверное значение для TADD"
		}
		return TADD(parts[1], value)
	case "TSEARCH":
		if len(parts) < 3 {
			return "Недостаточно параметров для TSEARCH"
		}
		value, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверное значение для TSEARCH"
		}
		return TSEARCH(parts[1], value)
	case "TDEL":
		if len(parts) < 3 {
			return "Недостаточно параметров для TDEL"
		}
		value, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверное значение для TDEL"
		}
		return TDEL(parts[1], value)
	case "TDELETE":
		if len(parts) < 3 {
			return "Недостаточно параметров для TDELETE"
		}
		value, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверное значение для TDELETE"
		}
		return TDELETE(parts[1], value)
	case "TGET":
		if len(parts) < 2 {
			return "Недостаточно параметров для TGET"
		}
		return TGET(parts[1])
	case "TGETSYM":
		if len(parts) < 2 {
			return "Недостаточно параметров для TGETSYM"
		}
		return TGETSYM(parts[1])
	case "TREAD":
		if len(parts) < 2 {
			return "Недостаточно параметров для TREAD"
		}
		return TREAD(parts[1])
		
	// Хеш-таблицы (H-команды)
	case "HCREATE":
		if len(parts) < 2 {
			return "Недостаточно параметров для HCREATE"
		}
		return HCREATE(parts[1])
	case "HADD":
		if len(parts) < 4 {
			return "Недостаточно параметров для HADD"
		}
		key, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверный ключ для HADD"
		}
		return HADD(parts[1], key, strings.Join(parts[3:], " "))
	case "HDEL":
		if len(parts) < 4 {
			return "Недостаточно параметров для HDEL"
		}
		key, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверный ключ для HDEL"
		}
		return HDEL(parts[1], key, strings.Join(parts[3:], " "))
	case "HSEARCH":
		if len(parts) < 4 {
			return "Недостаточно параметров для HSEARCH"
		}
		key, err := strconv.Atoi(parts[2])
		if err != nil {
			return "Неверный ключ для HSEARCH"
		}
		return HSEARCH(parts[1], key, strings.Join(parts[3:], " "))
	case "HGET":
		if len(parts) < 2 {
			return "Недостаточно параметров для HGET"
		}
		return HGET(parts[1])
	case "HSIZE":
		if len(parts) < 2 {
			return "Недостаточно параметров для HSIZE"
		}
		return HSIZE(parts[1])
		
	default:
		return "Неизвестная команда: " + command
	}
}

func main() {
	// Определяем флаги командной строки
	fileFlag := flag.String("file", "data.json", "Файл для сохранения/загрузки данных")
	queryFlag := flag.String("query", "", "Команда для выполнения")
	flag.Parse()

	filename := *fileFlag
	query := *queryFlag

	// Загружаем данные при старте
	loadFromFile(filename)

	if query != "" {
		// Режим выполнения одной команды
		result := processQuery(query)
		fmt.Println(result)
		
		// Сохраняем данные после выполнения команды
		saveToFile(filename)
	} else {
		// Если команда не указана, выводим справку
		fmt.Println("Использование: ./myapp --file файл.json --query 'команда'")
		fmt.Println("Пример: ./myapp --file data.json --query 'MCREATE myarray 10'")
		fmt.Println("Пример: ./myapp --file data.json --query 'MPUSHEND myarray значение'")
	}
}
