#include "tpMap.h"
/*
#include <stdlib.h>

const QMapDataBase QMapDataBase::shared_null = { Q_REFCOUNT_INITIALIZE_STATIC, 0, { 0, nullptr, nullptr }, nullptr };

const QMapNodeBase *QMapNodeBase::nextNode() const
{
    const QMapNodeBase *n = this;
    if (n->right) {
        n = n->right;
        while (n->left)
            n = n->left;
    } else {
        const QMapNodeBase *y = n->parent();
        while (y && n == y->right) {
            n = y;
            y = n->parent();
        }
        n = y;
    }
    return n;
}

const QMapNodeBase *QMapNodeBase::previousNode() const
{
    const QMapNodeBase *n = this;
    if (n->left) {
        n = n->left;
        while (n->right)
            n = n->right;
    } else {
        const QMapNodeBase *y = n->parent();
        while (y && n == y->left) {
            n = y;
            y = n->parent();
        }
        n = y;
    }
    return n;
}


void QMapDataBase::rotateLeft(QMapNodeBase *x)
{
    QMapNodeBase *&root = header.left;
    QMapNodeBase *y = x->right;
    x->right = y->left;
    if (y->left != nullptr)
        y->left->setParent(x);
    y->setParent(x->parent());
    if (x == root)
        root = y;
    else if (x == x->parent()->left)
        x->parent()->left = y;
    else
        x->parent()->right = y;
    y->left = x;
    x->setParent(y);
}


void QMapDataBase::rotateRight(QMapNodeBase *x)
{
    QMapNodeBase *&root = header.left;
    QMapNodeBase *y = x->left;
    x->left = y->right;
    if (y->right != nullptr)
        y->right->setParent(x);
    y->setParent(x->parent());
    if (x == root)
        root = y;
    else if (x == x->parent()->right)
        x->parent()->right = y;
    else
        x->parent()->left = y;
    y->right = x;
    x->setParent(y);
}


void QMapDataBase::rebalance(QMapNodeBase *x)
{
    QMapNodeBase *&root = header.left;
    x->setColor(QMapNodeBase::Red);
    while (x != root && x->parent()->color() == QMapNodeBase::Red) {
        if (x->parent() == x->parent()->parent()->left) {
            QMapNodeBase *y = x->parent()->parent()->right;
            if (y && y->color() == QMapNodeBase::Red) {
                x->parent()->setColor(QMapNodeBase::Black);
                y->setColor(QMapNodeBase::Black);
                x->parent()->parent()->setColor(QMapNodeBase::Red);
                x = x->parent()->parent();
            } else {
                if (x == x->parent()->right) {
                    x = x->parent();
                    rotateLeft(x);
                }
                x->parent()->setColor(QMapNodeBase::Black);
                x->parent()->parent()->setColor(QMapNodeBase::Red);
                rotateRight (x->parent()->parent());
            }
        } else {
            QMapNodeBase *y = x->parent()->parent()->left;
            if (y && y->color() == QMapNodeBase::Red) {
                x->parent()->setColor(QMapNodeBase::Black);
                y->setColor(QMapNodeBase::Black);
                x->parent()->parent()->setColor(QMapNodeBase::Red);
                x = x->parent()->parent();
            } else {
                if (x == x->parent()->left) {
                    x = x->parent();
                    rotateRight(x);
                }
                x->parent()->setColor(QMapNodeBase::Black);
                x->parent()->parent()->setColor(QMapNodeBase::Red);
                rotateLeft(x->parent()->parent());
            }
        }
    }
    root->setColor(QMapNodeBase::Black);
}

void QMapDataBase::freeNodeAndRebalance(QMapNodeBase *z)
{
    QMapNodeBase *&root = header.left;
    QMapNodeBase *y = z;
    QMapNodeBase *x;
    QMapNodeBase *x_parent;
    if (y->left == nullptr) {
        x = y->right;
        if (y == mostLeftNode) {
            if (x)
                mostLeftNode = x; // It cannot have (left) children due the red black invariant.
            else
                mostLeftNode = y->parent();
        }
    } else {
        if (y->right == nullptr) {
            x = y->left;
        } else {
            y = y->right;
            while (y->left != nullptr)
                y = y->left;
            x = y->right;
        }
    }
    if (y != z) {
        z->left->setParent(y);
        y->left = z->left;
        if (y != z->right) {
            x_parent = y->parent();
            if (x)
                x->setParent(y->parent());
            y->parent()->left = x;
            y->right = z->right;
            z->right->setParent(y);
        } else {
            x_parent = y;
        }
        if (root == z)
            root = y;
        else if (z->parent()->left == z)
            z->parent()->left = y;
        else
            z->parent()->right = y;
        y->setParent(z->parent());
        // Swap the colors
        QMapNodeBase::Color c = y->color();
        y->setColor(z->color());
        z->setColor(c);
        y = z;
    } else {
        x_parent = y->parent();
        if (x)
            x->setParent(y->parent());
        if (root == z)
            root = x;
        else if (z->parent()->left == z)
            z->parent()->left = x;
        else
            z->parent()->right = x;
    }
    if (y->color() != QMapNodeBase::Red) {
        while (x != root && (x == nullptr || x->color() == QMapNodeBase::Black)) {
            if (x == x_parent->left) {
                QMapNodeBase *w = x_parent->right;
                if (w->color() == QMapNodeBase::Red) {
                    w->setColor(QMapNodeBase::Black);
                    x_parent->setColor(QMapNodeBase::Red);
                    rotateLeft(x_parent);
                    w = x_parent->right;
                }
                if ((w->left == nullptr || w->left->color() == QMapNodeBase::Black) &&
                    (w->right == nullptr || w->right->color() == QMapNodeBase::Black)) {
                    w->setColor(QMapNodeBase::Red);
                    x = x_parent;
                    x_parent = x_parent->parent();
                } else {
                    if (w->right == nullptr || w->right->color() == QMapNodeBase::Black) {
                        if (w->left)
                            w->left->setColor(QMapNodeBase::Black);
                        w->setColor(QMapNodeBase::Red);
                        rotateRight(w);
                        w = x_parent->right;
                    }
                    w->setColor(x_parent->color());
                    x_parent->setColor(QMapNodeBase::Black);
                    if (w->right)
                        w->right->setColor(QMapNodeBase::Black);
                    rotateLeft(x_parent);
                    break;
                }
            } else {
            QMapNodeBase *w = x_parent->left;
            if (w->color() == QMapNodeBase::Red) {
                w->setColor(QMapNodeBase::Black);
                x_parent->setColor(QMapNodeBase::Red);
                rotateRight(x_parent);
                w = x_parent->left;
            }
            if ((w->right == nullptr || w->right->color() == QMapNodeBase::Black) &&
                (w->left == nullptr|| w->left->color() == QMapNodeBase::Black)) {
                w->setColor(QMapNodeBase::Red);
                x = x_parent;
                x_parent = x_parent->parent();
            } else {
                if (w->left == nullptr || w->left->color() == QMapNodeBase::Black) {
                    if (w->right)
                        w->right->setColor(QMapNodeBase::Black);
                    w->setColor(QMapNodeBase::Red);
                    rotateLeft(w);
                    w = x_parent->left;
                }
                w->setColor(x_parent->color());
                x_parent->setColor(QMapNodeBase::Black);
                if (w->left)
                    w->left->setColor(QMapNodeBase::Black);
                rotateRight(x_parent);
                break;
            }
        }
    }
    if (x)
        x->setColor(QMapNodeBase::Black);
    }
    free(y);
    --size;
}

void QMapDataBase::recalcMostLeftNode()
{
    mostLeftNode = &header;
    while (mostLeftNode->left)
        mostLeftNode = mostLeftNode->left;
}

static inline int32_t qMapAlignmentThreshold()
{
    // malloc on 32-bit platforms should return pointers that are 8-byte
    // aligned or more while on 64-bit platforms they should be 16-byte aligned
    // or more
    return 2 * sizeof(void*);
}

static inline void *qMapAllocate(int32_t alloc, int32_t alignment)
{
    return alignment > qMapAlignmentThreshold()
        ? qMallocAligned(alloc, alignment)
        : ::malloc(alloc);
}

static inline void qMapDeallocate(QMapNodeBase *node, int32_t alignment)
{
    if (alignment > qMapAlignmentThreshold())
        qFreeAligned(node);
    else
        ::free(node);
}

QMapNodeBase *QMapDataBase::createNode(int32_t alloc, int32_t alignment, QMapNodeBase *parent, bool left)
{
    QMapNodeBase *node = static_cast<QMapNodeBase *>(qMapAllocate(alloc, alignment));
    Q_CHECK_PTR(node);

    memset(node, 0, alloc);
    ++size;

    if (parent) {
        if (left) {
            parent->left = node;
            if (parent == mostLeftNode)
                mostLeftNode = node;
        } else {
            parent->right = node;
        }
        node->setParent(parent);
        rebalance(node);
    }
    return node;
}

void QMapDataBase::freeTree(QMapNodeBase *root, int32_t alignment)
{
    if (root->left)
        freeTree(root->left, alignment);
    if (root->right)
        freeTree(root->right, alignment);
    qMapDeallocate(root, alignment);
}

QMapDataBase *QMapDataBase::createData()
{
    QMapDataBase *d = new QMapDataBase;

    d->ref.initializeOwned();
    d->size = 0;

    d->header.p = 0;
    d->header.left = nullptr;
    d->header.right = nullptr;
    d->mostLeftNode = &(d->header);

    return d;
}

void QMapDataBase::freeData(QMapDataBase *d)
{
    delete d;
}
*/