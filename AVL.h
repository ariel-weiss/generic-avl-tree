/*
 * Created by Ariel Weiss
 *	13/05/18
 */
#pragma once
#define MAX(a,b) (((a)>(b))?(a):(b)); 

/*
 * Generic avl balanced search tree.
 * assumption:
 * [typename T] - The data in a tree's node:
 *		has default c'tor
 *		has copy c'tor
 *		has  =operator
 *		has  ==operator
 * [typename Comperator] - A class with ()operator (used as a key):
 *		has ()operator, that takes (T a, T b) and return 'true' if a>b
 */
template<typename T, typename Comperator>
class AVL
{
	//--Private fileds:--//

	class Node
	{
	public:
		T data;
		Node* left;
		Node* right;
		int height;
		Node() :left(NULL), right(NULL), height(0) {}
	};

	Node* main_root;
	Comperator isGreater;
	int size;
	//--Private functions:--//

	//-----------------Tree Maintenance-------------//
	/* Get the height */
	static int getHeight(const Node *node)
	{
		if (node == NULL) return  0;
		return node->height;
	}
	/* Set the height*/
	static void updateHeight(Node *root)
	{
		if (root == NULL) return;

		const int new_height = 1 + MAX((root->left != NULL ? root->left->height : 0),
			(root->right != NULL ? root->right->height : 0));
		root->height = new_height;

	}
	/* Return the balance factor of given tree: O(logn) */
	static int balanceFactor(const Node* node)
	{
		if (node == NULL) return 0;
		const int left = getHeight(node->left);//GetHeight(node->left);
		const int right = getHeight(node->right);//GetHeight(node->right);
		const auto bf = left - right;
		return bf;
	}

	/* insert new node into the tree : O(log(N)) */
	Node* insertAux(const T& data, Node *root)
	{
		if (root == NULL) {
			//allocate new node
			root = new Node();
			root->data = data;
			updateHeight(root);
			return root;
		}
		if (!isGreater(root->data,data)) {
			root->right = insertAux(data, root->right);
		}
		else
		{
			root->left = insertAux(data, root->left);
		}

		// Balancing:
		const int balance = balanceFactor(root);

		if (balance == 2) {
			if (balanceFactor(root->left) >= 0) {
				//left left case:
				root = rightRotation(root);
			}
			else //left right case: 
			{
				root->left = leftRotation(root->left);
				root = rightRotation(root);
			}
		}
		else if (balance == -2) {
			if (balanceFactor(root->right) <= 0) {
				//right right case:
				root = leftRotation(root);
			}
			else 
			{	//right left case:
				root->right = rightRotation(root->right);
				root = leftRotation(root);
			}
		}
		else
		{
			updateHeight(root);
		}
		return root;
	}
	/* return the leftmost child of given node. complexity: O(Log(N))*/
	Node* goLeft(Node* node)
	{
		if(node == NULL) return NULL;
		if (node->left == NULL) return node;
		return goLeft(node->left);
		
	}

	/* find the follower node of the given one. complexity: O(Log(N))*/
	Node* findFollower(Node * node)
	{
		if (node == NULL) return NULL;
		if (node->right == NULL) return NULL;
		return goLeft(node->right);
	}

	/* delete a node from the tree. complexity: O(log(N)) */
	Node* removeAux(const T& data, Node* root)
	{
		Node* temp;

		// Element not found
		if (root == NULL) return NULL;

		// Searching for element
		if (isGreater(root->data,data)) {
			root->left = removeAux(data, root->left);
		}
		else if (isGreater(data,root->data)) {
			root->right = removeAux(data, root->right);
		}

		// Element found!
		// Case 1: With 2 children:
		else if (root->left != NULL && root->right != NULL)
		{
			//replace node with it follower, and remove the follower
			temp = findFollower(root);
			root->data = temp->data;
			root->right = removeAux(root->data, root->right);
		}
		// Case 2: With one or zero child:
		else
		{
			//if there is a child, connect it with the node's parent
			//then, delete the node
			temp = root;
			if (root->right != NULL && root->left == NULL)
				root = root->right;
			else if (root->left != NULL && root->right == NULL)
				root = root->left;
			else
				root = NULL;
			delete temp;
			temp = NULL;
		}

		if (root == NULL)
			return root;

		updateHeight(root);
		// Balancing:
		const int balance = balanceFactor(root);
		// If left node is deleted, right case
		if (balance == -2)
		{
			// right right case
			if (balanceFactor(root->left) == 1)
				return leftRotation(root);
			// right left case
			else {
				root->right = rightRotation(root->right);
				return leftRotation(root);
			}
		}
		// If right node is deleted, left case
		else if (balance == 2)
		{
			// left left case
			if (balanceFactor(root->right) == -1)
				return rightRotation(root);
			// left right case
			else {
				root->left = leftRotation(root->left);
				return rightRotation(root);
			}
		}
		
		return root;
	}

	/* ----Rotations:----- */
	/* Rotate right. complexity: O(1) */
	Node* rightRotation(Node* &root)
	{
		if (root == NULL) return root;
		//MEM LEAK: What if no left son?
		Node *new_root = root->left;
		if (new_root == NULL) return root;
		root->left = new_root->right;
		new_root->right = root;
		updateHeight(root);
		updateHeight(new_root);
		return new_root;
	}
	/* Rotate left. complexity: O(1) */
	Node* leftRotation(Node* &root)
	{
		if (root == NULL) return root;
		Node *new_root = root->right;
		if (new_root == NULL) return root;
		root->right = new_root->left;
		new_root->left = root;
		updateHeight(root);
		updateHeight(new_root);
		return new_root;
	}

	//-----------------Tree Functions------------//

	//Inorder exporting to T array:
	void inorderExport(int& index,T* data_array,Node* t)
	{
		if (t == NULL)
			return;
		if (t->left != NULL)
			inorderExport(index, data_array, t->left);
		
		if (index > size) return;
		data_array[index] =  t->data; 
		index++;
		
		if (t->right != NULL)
			inorderExport(index, data_array, t->right);
	}
	/* Delete the tree recursevly, postorder way. complexity: O(n) */
	void deletionAux(Node* t)
	{
		if (t == NULL)
			return;
		if (t->left != NULL)
			deletionAux(t->left);
		if (t->right != NULL)
			deletionAux(t->right);

		delete t;
	}
	/* Check if element exsits in the tree. complexity: O(Log(n)) */
	bool isExistsAux(const T& data,Node* root)
	{
		bool found = false;
		if (root == NULL) return false;
		if (root->data == data) return true;
		
		if(isGreater(root->data,data))
		{
			found = isExistsAux(data, root->left);
		}
		else {
			found = isExistsAux(data, root->right);
		}
		return found;
	}
	/* Return a ptr to spcific node's data. complexity: O(Log(n)) */
	T* findAux(const T& data, Node* root)
	{
		
		if (root == NULL) return NULL;
		if (root->data == data) return &(root->data);

		if (isGreater(root->data, data))
		{
			return findAux(data, root->left);
		}
		else
		{
			return findAux(data, root->right);
		}
		
	}
	/* 
	 * Do an action on every node, inorder way. complexity: O(n)
	 * assumption:
	 * [Action action] - A class with ()operator, that receives [T data] as parameter.
	 */
	template<typename Action>
	void inorderActionAux(Action action, Node* root)
	{
		if (root == NULL) return;

		if (root->left != NULL)
			inorderActionAux(action, root->left);

		action(root->data);

		if (root->right != NULL)
			inorderActionAux(action, root->right);
	}
	/* Creates new node, with a given height */
	Node* newNode(T data, int height)
	{
		Node* node = new Node;
		node->data = data;
		node->left = NULL;
		node->right = NULL;
		node->height = height;
		++(this->size);
		return node;
	}
	/* A function that generates Balanced Binary Search Tree from a sorted array */
	Node* generateFromArrayAux(T* arr, int start, int end, int height)
	{
		if (start > end) return NULL;

		int mid = (start + end) / 2;
		Node* root = newNode(arr[mid], height);

		root->left = generateFromArrayAux(arr, start, mid - 1, height - 1);

		root->right = generateFromArrayAux(arr, mid + 1, end, height - 1);

		return root;
	}

	public:

	//---------------C'tors----------------//
	AVL():main_root(), isGreater(),size(0)	{	}

	~AVL(){	}


	//---------------Actions----------------//
	void insert(const T& data)
	{
		if(isExists(data))
		{
			throw AlreadyExists();
		}
		
		main_root = insertAux(data, main_root);
		size++;
	}

	void remove(const T& data)
	{
		if (!isExists(data))
		{
			throw DoesNotExists();
		}

		main_root = removeAux(data, main_root);
		size--;
	}
	
	void destroy()
	{
		deletionAux(main_root);
		main_root = NULL;
		size = 0;
	}

	bool isExists(const T& data)
	{
		return isExistsAux(data, main_root);
	}
	
	T* find(const T& data) 
	{
		T* d = findAux(data, main_root);
		if (d == NULL) throw DoesNotExists();
		return	d;
	}
	
	/* intuitive way to use the 'find' function: */
	T* operator[](const T& data)
	{
		return find(data);
	}

	int getSize() const
	{
		return size;
	}

	T* exportData() 
	{
		T* data = new T[size];
		int index = 0;
		inorderExport(index, data, main_root);
		return data;
	}

	template<typename Action>
	void inorderAction(Action action)
	{
		inorderActionAux(action, main_root);
	}
	
	/*
	 * Merging with a given tree from THE SAME type into array. complexity: O(N+M)
	 * -only elements that have passed the condition are inserted.
	 * assumption:
	 * [Condition cond] - A class with ()operator, that receives [T data] as parameter.
	 */
	template<typename Condition>
	void getMergedArray(T** mergedArr, AVL<T, Comperator>& tree2,Condition cond)
	{
		T* arr1 = exportData();
		T* arr2 = tree2.exportData();
		const int m = getSize(), n = tree2.getSize();
		int i = 0, j = 0, k = 0;

		//Traverse through both arrays
		while (i < m && j < n)
		{
			//put the smaller element in mergedArr
			if (isGreater(arr2[j], arr1[i]))
			{
				if(cond(arr1[i]))
				{
					(*mergedArr)[k] = arr1[i];
					k++;
				}
					
				i++;
			}
			else
			{
				if (cond(arr2[j]))
				{
					(*mergedArr)[k] = arr2[j];
					k++;
				}
				j++;
			}
			
		}
		// If there are more elements in first array
		while (i < m)
		{
			if (cond(arr1[i]))
			{
				(*mergedArr)[k] = arr1[i];
				k++;
			}
				
			i++; 
		}
		// If there are more elements in second array
		while (j < n)
		{
			if (cond(arr2[j]))
			{
				(*mergedArr)[k] = arr2[j];
				k++;
			}
			j++; 
		}

		delete[] arr1;
		delete[] arr2;

	}

	/* Generate a new tree from sorted array. complexity: O(Log(n)) */
	void generateFromArray( T* arr,const int size)
	{
		this->size = 0;
		main_root = generateFromArrayAux(arr, 0, size, (size / 2));
	}
	
};

