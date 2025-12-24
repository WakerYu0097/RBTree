#include"RBTree.cpp"
void testRBTree(int nodeCount, int seed, int deleteCount = 0, int startIndex = 0)
{
    using namespace std;

    startIndex = max(0, min(startIndex, nodeCount - 1));
    deleteCount = min(deleteCount, nodeCount - startIndex);

    bool test_delete = deleteCount != 0;

    RBTree<int, string> tree;
    mt19937 random(seed);
    uniform_int_distribution<int> dist(1, 100);
    vector<int> numsToDelete;

    for (int i = 0; i < nodeCount; i++)
    {
        int cur = dist(random);
        if (deleteCount > 0 && i >= startIndex)
        {
            deleteCount--;
            if (find(numsToDelete.begin(), numsToDelete.end(), cur) == numsToDelete.end())
            {
                numsToDelete.push_back(cur);
            }
        }
        tree.Insert(cur, to_string(cur));
        cout << cur << " ";
    }
    cout << endl;
    tree.Print();

    if (test_delete)
    {
        while (!numsToDelete.empty())
        {
            int num = numsToDelete.back();
            numsToDelete.pop_back();
            tree.Delete(num);
            cout << "After Delete " << num << ":" << endl;
            tree.Print();
        }
    }
}

void testSetTree(int nodeCount, int seed, int deleteCount = 0, int startIndex = 0)
{
    using namespace std;

    startIndex = max(0, min(startIndex, nodeCount - 1));
    deleteCount = min(deleteCount, nodeCount - startIndex);

    bool test_delete = deleteCount != 0;
    bool showNums = nodeCount <= 1024;

    SetTree<int> tree;
    mt19937 random(seed);
    uniform_int_distribution<int> dist(1, 100);
    vector<int> numsToDelete;

    for (int i = 0; i < nodeCount; i++)
    {
        int cur = dist(random);
        if (deleteCount > 0 && i >= startIndex)
        {
            deleteCount--;
            if (find(numsToDelete.begin(), numsToDelete.end(), cur) == numsToDelete.end())
            {
                numsToDelete.push_back(cur);
            }
        }
        tree.Insert(cur);
        if (showNums)
        {
            cout << cur << " ";
        }
    }
    if (showNums)
        cout << endl;
    tree.Print();

    if (test_delete)
    {
        while (!numsToDelete.empty())
        {
            int num = numsToDelete.back();
            numsToDelete.pop_back();
            tree.Delete(num);
            cout << "After Delete " << num << ":" << endl;
            tree.Print();
        }
    }
}

int main()
{
    using namespace std;

    cout << "RBTree Test" << endl;
    cout << "==================================" << endl;
    testRBTree(32, 50, 0);
    cout << endl;

    cout << "SetTree Test" << endl;
    cout << "==================================" << endl;
    testSetTree(32, 50, 0);
    cout << endl;

    system("pause");
    return 0;
}
