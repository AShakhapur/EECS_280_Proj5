// Project UID db1f506d06d84ab787baf250c265e24e

#include "BinarySearchTree.h"
#include "unit_test_framework.h"
#include <sstream>

TEST(test_tree_size) {
    BinarySearchTree<int> tree;

    ASSERT_TRUE(tree.empty());

    ASSERT_TRUE(tree.size() == 0);
    ASSERT_TRUE(tree.height() == 0);

    tree.insert(4);

    ASSERT_TRUE(tree.size() == 1);
    ASSERT_TRUE(tree.height() == 1);
    ASSERT_EQUAL(*tree.max_element(), *tree.min_element());

    ASSERT_TRUE(tree.find(4) != tree.end());

    tree.insert(8);
    tree.insert(2);

    ASSERT_TRUE(tree.height() == 2);

    ASSERT_TRUE(tree.check_sorting_invariant()); 
    ASSERT_TRUE(*tree.max_element() == 8);
    ASSERT_TRUE(*tree.min_element() == 2);
    ASSERT_TRUE(*tree.min_greater_than(4) == 8);
    ASSERT_TRUE(*tree.min_greater_than(2) == 4);
}

TEST(test_big_three) {

    BinarySearchTree<string> tree;
    const BinarySearchTree<string>& const_tree = tree;

    tree.insert("aa");
    tree.insert("bb");
    tree.insert("cc");

    // Big Three
    BinarySearchTree<string> tree_copy(const_tree);
    tree_copy = const_tree;

    ASSERT_TRUE(tree.height() == const_tree.height());
    ASSERT_EQUAL(tree.size(), const_tree.size());
    ASSERT_EQUAL(const_tree.to_string(), tree.to_string());

}

TEST(test_check_sorting_invariant_basic) {
    BinarySearchTree<int> b;
    b.insert(1);
    b.insert(0);
    ASSERT_TRUE(b.check_sorting_invariant());

    // change first datum to 2, resulting in the first broken tree above
    *b.begin() = 2;
    ASSERT_TRUE(b.check_sorting_invariant());
}

TEST(test_check_sorting_invariant) {

    BinarySearchTree<int> tree;
    bool b;

    tree.insert(5);
    tree.insert(14);
    tree.insert(10);
    tree.insert(22);
    tree.insert(19);
    tree.insert(2);
    tree.insert(30);
    tree.insert(35);

    b = tree.check_sorting_invariant();
    ASSERT_TRUE(b);

    tree.insert(1);
    b = tree.check_sorting_invariant();
    ASSERT_TRUE(b);


    ostringstream oss_preorder;
    tree.traverse_preorder(oss_preorder);
    cout << "preorder" << endl;
    cout << oss_preorder.str() << endl << endl;
    ASSERT_TRUE(oss_preorder.str() == "5 2 1 14 10 22 19 30 35 ");

    ostringstream oss_inorder;
    tree.traverse_inorder(oss_inorder);
    cout << "inorder" << endl;
    cout << oss_inorder.str() << endl << endl;
    ASSERT_TRUE(oss_inorder.str() == "1 2 5 10 14 19 22 30 35 ");

}


TEST(test_not_existing_nodes) {
    BinarySearchTree<int> tree;
    
    ASSERT_TRUE(tree.find(7) == tree.end());

    tree.insert(1);
    tree.insert(11);
    tree.insert(21);

    ASSERT_TRUE(tree.find(7) == tree.end());
    ASSERT_TRUE(tree.min_greater_than(111) == tree.end());
    
}

//TEST() { (map)

//}

TEST_MAIN()