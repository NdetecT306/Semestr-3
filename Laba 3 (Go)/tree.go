package main

import (
	"fmt"
)

// Tree представляет узел красно-черного дерева
type Tree struct {
	value  int
	color  Color
	left   *Tree
	right  *Tree
	parent *Tree
}

// Color определяет цвет узла
type Color bool

const (
	RED   Color = false
	BLACK Color = true
)

// NewTree создает новый узел дерева
func NewTree() *Tree {
	return &Tree{
		color:  RED,
		left:   nil,
		right:  nil,
		parent: nil,
	}
}

// Getters
func (t *Tree) getValue() int {
	return t.value
}

func (t *Tree) getColor() Color {
	return t.color
}

func (t *Tree) getLeft() *Tree {
	return t.left
}

func (t *Tree) getRight() *Tree {
	return t.right
}

func (t *Tree) getParent() *Tree {
	return t.parent
}

// CreateRoot создает корень дерева
func CreateRoot(k int) *Tree {
	root := NewTree()
	root.value = k
	root.color = BLACK
	return root
}

// CreateNode создает новый узел (лист)
func (t *Tree) CreateNode(k int) *Tree {
	node := NewTree()
	node.value = k
	return node
}

// IfValueExists проверяет существование значения в дереве
func IfValueExists(root *Tree, k int) bool {
	if root == nil {
		return false
	}
	if root.value == k {
		return true
	}
	if IfValueExists(root.left, k) {
		return true
	}
	if IfValueExists(root.right, k) {
		return true
	}
	return false
}

// rotateLeft выполняет левый поворот
func (t *Tree) rotateLeft(root **Tree, x *Tree) *Tree {
	y := x.right
	x.right = y.left

	if y.left != nil {
		y.left.parent = x
	}

	y.parent = x.parent

	if x.parent == nil {
		*root = y
	} else if x == x.parent.left {
		x.parent.left = y
	} else {
		x.parent.right = y
	}

	y.left = x
	x.parent = y

	return y
}

// rotateRight выполняет правый поворот
func (t *Tree) rotateRight(root **Tree, y *Tree) *Tree {
	x := y.left
	y.left = x.right

	if x.right != nil {
		x.right.parent = y
	}

	x.parent = y.parent

	if y.parent == nil {
		*root = x
	} else if y == y.parent.left {
		y.parent.left = x
	} else {
		y.parent.right = x
	}

	x.right = y
	y.parent = x

	return x
}

// fixInsert балансировка после вставки
func (t *Tree) fixInsert(root **Tree, ptr *Tree) {
	for ptr != *root && ptr.parent != nil && ptr.parent.color == RED {
		parent := ptr.parent
		grandpa := parent.parent

		if parent == grandpa.left { // Левый потомок
			uncle := grandpa.right
			if uncle != nil && uncle.color == RED { // ДЯДЯ КРАСНЫЙ (случай 2)
				grandpa.color = RED
				parent.color = BLACK
				uncle.color = BLACK
				ptr = grandpa
			} else {
				if ptr == parent.right { // Для случая 4, когда нужны 2 ротации
					ptr = parent
					t.rotateLeft(root, ptr)
					parent = ptr.parent
				}
				parent.color = BLACK // Случай 3
				grandpa.color = RED
				t.rotateRight(root, grandpa) // Ротация справа
			}
		} else { // Правый потомок
			uncle := grandpa.left
			if uncle != nil && uncle.color == RED {
				grandpa.color = RED
				parent.color = BLACK
				uncle.color = BLACK
				ptr = grandpa
			} else {
				if ptr == parent.left { // Для случая 4, когда нужны 2 ротации
					ptr = parent
					t.rotateRight(root, ptr)
					parent = ptr.parent
				}
				parent.color = BLACK // Случай 3
				grandpa.color = RED
				t.rotateLeft(root, grandpa) // Ротация слева
			}
		}
	}
	(*root).color = BLACK // Корень всегда черный (случай 1)
}

// AddLeaf добавляет узел в дерево
func AddLeaf(root *Tree, k int) *Tree {
	if root == nil {
		return CreateRoot(k)
	}

	element := NewTree()
	element.value = k

	peremElem := root
	var parent *Tree = nil

	// Поиск места вставки
	for peremElem != nil {
		parent = peremElem
		if k < peremElem.value {
			peremElem = peremElem.left
		} else {
			peremElem = peremElem.right
		}
	}

	element.parent = parent
	if k < parent.value {
		parent.left = element
	} else {
		parent.right = element
	}

	// Создаем временный Tree для вызова методов
	tempTree := &Tree{}
	tempTree.fixInsert(&root, element)
	return root
}

// findPlace ищет узел по значению
func (t *Tree) findPlace(root *Tree, k int) *Tree {
	if root == nil || root.value == k {
		return root
	}
	if k < root.value {
		return t.findPlace(root.left, k)
	}
	return t.findPlace(root.right, k)
}

// FindMinimum ищет минимальный узел в поддереве
func FindMinimum(node *Tree) *Tree {
	if node == nil {
		return nil
	}
	for node.left != nil {
		node = node.left
	}
	return node
}

// FindMax ищет максимальный узел в поддереве
func FindMax(node *Tree) *Tree {
	if node == nil {
		return nil
	}
	for node.right != nil {
		node = node.right
	}
	return node
}

// transplant трансплантирует один узел на место другого
func (t *Tree) transplant(root **Tree, u, v *Tree) {
	if u.parent == nil {
		*root = v
	} else if u == u.parent.left {
		u.parent.left = v
	} else {
		u.parent.right = v
	}

	if v != nil {
		v.parent = u.parent
	}
}

// fixDelete балансировка после удаления
func (t *Tree) fixDelete(root **Tree, x *Tree, parent *Tree) {
	for x != *root && (x == nil || x.color == BLACK) {
		if parent == nil {
			break
		}

		if x == parent.left { // Левый ребенок
			sibling := parent.right
			if sibling == nil {
				break
			}

			if sibling.color == RED { // Случай 1: брат красный
				sibling.color = BLACK
				parent.color = RED
				t.rotateLeft(root, parent)
				sibling = parent.right
				if sibling == nil {
					break
				}
			}

			leftBlack := sibling.left == nil || sibling.left.color == BLACK
			rightBlack := sibling.right == nil || sibling.right.color == BLACK

			if leftBlack && rightBlack { // Случай 2: оба ребенка брата черные
				sibling.color = RED
				x = parent
				parent = x.parent
			} else {
				if rightBlack { // Случай 3: правый ребенок брата черный
					if sibling.left != nil {
						sibling.left.color = BLACK
					}
					sibling.color = RED
					t.rotateRight(root, sibling)
					sibling = parent.right
					if sibling == nil {
						break
					}
				}

				// Случай 4: правый ребенок брата красный
				sibling.color = parent.color
				parent.color = BLACK
				if sibling.right != nil {
					sibling.right.color = BLACK
				}
				t.rotateLeft(root, parent)
				x = *root
				break
			}
		} else { // Правый ребенок
			sibling := parent.left
			if sibling == nil {
				break
			}

			if sibling.color == RED {
				sibling.color = BLACK
				parent.color = RED
				t.rotateRight(root, parent)
				sibling = parent.left
				if sibling == nil {
					break
				}
			}

			leftBlack := sibling.left == nil || sibling.left.color == BLACK
			rightBlack := sibling.right == nil || sibling.right.color == BLACK

			if leftBlack && rightBlack {
				sibling.color = RED
				x = parent
				parent = x.parent
			} else {
				if leftBlack {
					if sibling.right != nil {
						sibling.right.color = BLACK
					}
					sibling.color = RED
					t.rotateLeft(root, sibling)
					sibling = parent.left
					if sibling == nil {
						break
					}
				}

				sibling.color = parent.color
				parent.color = BLACK
				if sibling.left != nil {
					sibling.left.color = BLACK
				}
				t.rotateRight(root, parent)
				x = *root
				break
			}
		}
	}

	if x != nil {
		x.color = BLACK
	}
}

// deleteLeaf удаляет лист
func (t *Tree) deleteLeaf(root **Tree, k int) bool {
	z := t.findPlace(*root, k)
	if z == nil {
		fmt.Printf("Узел со значением %d не найден!\n", k)
		return false
	}

	y := z
	var x *Tree = nil
	yOriginalColor := y.color

	if z.left == nil {
		x = z.right
		t.transplant(root, z, z.right)
	} else if z.right == nil {
		x = z.left
		t.transplant(root, z, z.left)
	}

	if yOriginalColor == BLACK {
		t.fixDelete(root, x, z.parent)
	}

	return true
}

// deleteNodeWithOneChild удаляет узел с одним ребенком
func (t *Tree) deleteNodeWithOneChild(root **Tree, k int) bool {
	z := t.findPlace(*root, k)
	if z == nil {
		fmt.Printf("Узел со значением %d не найден!\n", k)
		return false
	}

	childCount := 0
	if z.left != nil {
		childCount++
	}
	if z.right != nil {
		childCount++
	}

	if childCount != 1 {
		fmt.Printf("Узел %d имеет не одного ребенка! Удаление невозможно.\n", k)
		return false
	}

	var child *Tree
	if z.left != nil {
		child = z.left
	} else {
		child = z.right
	}

	parent := z.parent
	originalColor := z.color

	t.transplant(root, z, child)

	if originalColor == BLACK {
		if child != nil && child.color == RED {
			child.color = BLACK
		} else {
			t.fixDelete(root, child, parent)
		}
	}

	return true
}

// deleteNodeWithTwoChildren удаляет узел с двумя детьми
func (t *Tree) deleteNodeWithTwoChildren(root **Tree, k int) bool {
	z := t.findPlace(*root, k)
	if z == nil {
		fmt.Printf("Узел со значением %d не найден!\n", k)
		return false
	}

	successor := FindMax(z.left)
	successorOriginalColor := successor.color

	// Меняем значения
	tempValue := z.value
	z.value = successor.value
	successor.value = tempValue

	var x *Tree
	if successor.left == nil {
		x = successor.right
	} else {
		x = successor.left
	}

	t.transplant(root, successor, x)

	if successorOriginalColor == BLACK {
		t.fixDelete(root, x, successor.parent)
	}

	return true
}

// DeleteNode универсальная функция удаления любого узла
func DeleteNode(root *Tree, k int) *Tree {
	if root == nil {
		fmt.Println("Дерево пустое!")
		return root
	}

	z := (&Tree{}).findPlace(root, k)
	if z == nil {
		fmt.Printf("Узел со значением %d не найден!\n", k)
		return root
	}

	// Определяем тип узла и вызываем соответствующую функцию
	tempTree := &Tree{}
	if z.left == nil && z.right == nil {
		tempTree.deleteLeaf(&root, k)
	} else if (z.left != nil && z.right == nil) || (z.left == nil && z.right != nil) {
		tempTree.deleteNodeWithOneChild(&root, k)
	} else {
		tempTree.deleteNodeWithTwoChildren(&root, k)
	}

	return root
}

// Symmetrical обход симметричный (in-order)
func (t *Tree) Symmetrical(root *Tree) {
	if root == nil {
		return
	}
	t.Symmetrical(root.left)
	colorStr := "r"
	if root.color == BLACK {
		colorStr = "b"
	}
	fmt.Printf("%d%s ", root.value, colorStr)
	t.Symmetrical(root.right)
}

// FromTopToBottom обход сверху вниз (pre-order)
func (t *Tree) FromTopToBottom(root *Tree) {
	if root == nil {
		return
	}
	colorStr := "r"
	if root.color == BLACK {
		colorStr = "b"
	}
	fmt.Printf("%d%s ", root.value, colorStr)
	t.FromTopToBottom(root.left)
	t.FromTopToBottom(root.right)
}

// FromBottomToTop обход снизу вверх (post-order)
func (t *Tree) FromBottomToTop(root *Tree) {
	if root == nil {
		return
	}
	t.FromBottomToTop(root.left)
	t.FromBottomToTop(root.right)
	colorStr := "r"
	if root.color == BLACK {
		colorStr = "b"
	}
	fmt.Printf("%d%s ", root.value, colorStr)
}

// GetHeight возвращает высоту дерева
func (t *Tree) GetHeight(root *Tree) int {
	if root == nil {
		return 0
	}
	leftHeight := t.GetHeight(root.left)
	rightHeight := t.GetHeight(root.right)

	if leftHeight > rightHeight {
		return leftHeight + 1
	}
	return rightHeight + 1
}

// BFSRecursiveHelper вспомогательная функция для BFS
func (t *Tree) BFSRecursiveHelper(root *Tree, level int) {
	if root == nil {
		return
	}
	if level == 1 {
		colorStr := "r"
		if root.color == BLACK {
			colorStr = "b"
		}
		fmt.Printf("%d%s ", root.value, colorStr)
	} else if level > 1 {
		t.BFSRecursiveHelper(root.left, level-1)
		t.BFSRecursiveHelper(root.right, level-1)
	}
}

// BFS обход в ширину
func (t *Tree) BFS(root *Tree) {
	if root == nil {
		fmt.Println("Дерево пустое!")
		return
	}

	height := t.GetHeight(root)
	for i := 1; i <= height; i++ {
		t.BFSRecursiveHelper(root, i)
	}
	fmt.Println()
}
