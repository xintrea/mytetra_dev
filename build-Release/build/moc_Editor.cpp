/****************************************************************************
** Meta object code from reading C++ file 'Editor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/libraries/wyedit/Editor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Editor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Editor_t {
    QByteArrayData data[70];
    char stringdata0[1412];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Editor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Editor_t qt_meta_stringdata_Editor = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Editor"
QT_MOC_LITERAL(1, 7, 23), // "send_set_textindent_pos"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 1), // "i"
QT_MOC_LITERAL(4, 34, 23), // "send_set_leftindent_pos"
QT_MOC_LITERAL(5, 58, 24), // "send_set_rightindent_pos"
QT_MOC_LITERAL(6, 83, 21), // "send_expand_edit_area"
QT_MOC_LITERAL(7, 105, 4), // "flag"
QT_MOC_LITERAL(8, 110, 23), // "wyeditFindInBaseClicked"
QT_MOC_LITERAL(9, 134, 30), // "onUpdateIndentlineGeometrySlot"
QT_MOC_LITERAL(10, 165, 15), // "on_bold_clicked"
QT_MOC_LITERAL(11, 181, 17), // "on_italic_clicked"
QT_MOC_LITERAL(12, 199, 20), // "on_underline_clicked"
QT_MOC_LITERAL(13, 220, 20), // "on_monospace_clicked"
QT_MOC_LITERAL(14, 241, 15), // "on_code_clicked"
QT_MOC_LITERAL(15, 257, 16), // "on_clear_clicked"
QT_MOC_LITERAL(16, 274, 22), // "on_numericlist_clicked"
QT_MOC_LITERAL(17, 297, 18), // "on_dotlist_clicked"
QT_MOC_LITERAL(18, 316, 21), // "on_indentplus_clicked"
QT_MOC_LITERAL(19, 338, 22), // "on_indentminus_clicked"
QT_MOC_LITERAL(20, 361, 20), // "on_alignleft_clicked"
QT_MOC_LITERAL(21, 382, 22), // "on_aligncenter_clicked"
QT_MOC_LITERAL(22, 405, 21), // "on_alignright_clicked"
QT_MOC_LITERAL(23, 427, 21), // "on_alignwidth_clicked"
QT_MOC_LITERAL(24, 449, 21), // "on_fontselect_changed"
QT_MOC_LITERAL(25, 471, 4), // "font"
QT_MOC_LITERAL(26, 476, 19), // "on_fontsize_changed"
QT_MOC_LITERAL(27, 496, 20), // "on_fontcolor_clicked"
QT_MOC_LITERAL(28, 517, 25), // "set_fontselect_on_display"
QT_MOC_LITERAL(29, 543, 8), // "fontName"
QT_MOC_LITERAL(30, 552, 23), // "set_fontsize_on_display"
QT_MOC_LITERAL(31, 576, 1), // "n"
QT_MOC_LITERAL(32, 578, 19), // "on_showhtml_clicked"
QT_MOC_LITERAL(33, 598, 19), // "on_findtext_clicked"
QT_MOC_LITERAL(34, 618, 19), // "on_settings_clicked"
QT_MOC_LITERAL(35, 638, 25), // "on_showformatting_clicked"
QT_MOC_LITERAL(36, 664, 22), // "on_createtable_clicked"
QT_MOC_LITERAL(37, 687, 27), // "on_table_remove_row_clicked"
QT_MOC_LITERAL(38, 715, 27), // "on_table_remove_col_clicked"
QT_MOC_LITERAL(39, 743, 24), // "on_table_add_row_clicked"
QT_MOC_LITERAL(40, 768, 24), // "on_table_add_col_clicked"
QT_MOC_LITERAL(41, 793, 28), // "on_table_merge_cells_clicked"
QT_MOC_LITERAL(42, 822, 27), // "on_table_split_cell_clicked"
QT_MOC_LITERAL(43, 850, 33), // "on_insert_image_from_file_cli..."
QT_MOC_LITERAL(44, 884, 27), // "on_expand_edit_area_clicked"
QT_MOC_LITERAL(45, 912, 29), // "on_expand_tools_lines_clicked"
QT_MOC_LITERAL(46, 942, 15), // "on_save_clicked"
QT_MOC_LITERAL(47, 958, 15), // "on_back_clicked"
QT_MOC_LITERAL(48, 974, 23), // "on_find_in_base_clicked"
QT_MOC_LITERAL(49, 998, 20), // "on_show_text_clicked"
QT_MOC_LITERAL(50, 1019, 20), // "on_to_attach_clicked"
QT_MOC_LITERAL(51, 1040, 26), // "on_cursor_position_changed"
QT_MOC_LITERAL(52, 1067, 20), // "on_selection_changed"
QT_MOC_LITERAL(53, 1088, 7), // "on_undo"
QT_MOC_LITERAL(54, 1096, 7), // "on_redo"
QT_MOC_LITERAL(55, 1104, 6), // "on_cut"
QT_MOC_LITERAL(56, 1111, 7), // "on_copy"
QT_MOC_LITERAL(57, 1119, 8), // "on_paste"
QT_MOC_LITERAL(58, 1128, 12), // "on_selectAll"
QT_MOC_LITERAL(59, 1141, 25), // "on_findtext_signal_detect"
QT_MOC_LITERAL(60, 1167, 4), // "text"
QT_MOC_LITERAL(61, 1172, 24), // "QTextDocument::FindFlags"
QT_MOC_LITERAL(62, 1197, 5), // "flags"
QT_MOC_LITERAL(63, 1203, 35), // "on_indentline_change_textinde..."
QT_MOC_LITERAL(64, 1239, 35), // "on_indentline_change_leftinde..."
QT_MOC_LITERAL(65, 1275, 36), // "on_indentline_change_rightind..."
QT_MOC_LITERAL(66, 1312, 27), // "on_indentline_mouse_release"
QT_MOC_LITERAL(67, 1340, 29), // "on_customContextMenuRequested"
QT_MOC_LITERAL(68, 1370, 3), // "pos"
QT_MOC_LITERAL(69, 1374, 37) // "on_context_menu_edit_image_pr..."

    },
    "Editor\0send_set_textindent_pos\0\0i\0"
    "send_set_leftindent_pos\0"
    "send_set_rightindent_pos\0send_expand_edit_area\0"
    "flag\0wyeditFindInBaseClicked\0"
    "onUpdateIndentlineGeometrySlot\0"
    "on_bold_clicked\0on_italic_clicked\0"
    "on_underline_clicked\0on_monospace_clicked\0"
    "on_code_clicked\0on_clear_clicked\0"
    "on_numericlist_clicked\0on_dotlist_clicked\0"
    "on_indentplus_clicked\0on_indentminus_clicked\0"
    "on_alignleft_clicked\0on_aligncenter_clicked\0"
    "on_alignright_clicked\0on_alignwidth_clicked\0"
    "on_fontselect_changed\0font\0"
    "on_fontsize_changed\0on_fontcolor_clicked\0"
    "set_fontselect_on_display\0fontName\0"
    "set_fontsize_on_display\0n\0on_showhtml_clicked\0"
    "on_findtext_clicked\0on_settings_clicked\0"
    "on_showformatting_clicked\0"
    "on_createtable_clicked\0"
    "on_table_remove_row_clicked\0"
    "on_table_remove_col_clicked\0"
    "on_table_add_row_clicked\0"
    "on_table_add_col_clicked\0"
    "on_table_merge_cells_clicked\0"
    "on_table_split_cell_clicked\0"
    "on_insert_image_from_file_clicked\0"
    "on_expand_edit_area_clicked\0"
    "on_expand_tools_lines_clicked\0"
    "on_save_clicked\0on_back_clicked\0"
    "on_find_in_base_clicked\0on_show_text_clicked\0"
    "on_to_attach_clicked\0on_cursor_position_changed\0"
    "on_selection_changed\0on_undo\0on_redo\0"
    "on_cut\0on_copy\0on_paste\0on_selectAll\0"
    "on_findtext_signal_detect\0text\0"
    "QTextDocument::FindFlags\0flags\0"
    "on_indentline_change_textindent_pos\0"
    "on_indentline_change_leftindent_pos\0"
    "on_indentline_change_rightindent_pos\0"
    "on_indentline_mouse_release\0"
    "on_customContextMenuRequested\0pos\0"
    "on_context_menu_edit_image_properties"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Editor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      59,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  309,    2, 0x06 /* Public */,
       4,    1,  312,    2, 0x06 /* Public */,
       5,    1,  315,    2, 0x06 /* Public */,
       6,    1,  318,    2, 0x06 /* Public */,
       8,    0,  321,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,  322,    2, 0x0a /* Public */,
      10,    0,  323,    2, 0x08 /* Private */,
      11,    0,  324,    2, 0x08 /* Private */,
      12,    0,  325,    2, 0x08 /* Private */,
      13,    0,  326,    2, 0x08 /* Private */,
      14,    0,  327,    2, 0x08 /* Private */,
      15,    0,  328,    2, 0x08 /* Private */,
      16,    0,  329,    2, 0x08 /* Private */,
      17,    0,  330,    2, 0x08 /* Private */,
      18,    0,  331,    2, 0x08 /* Private */,
      19,    0,  332,    2, 0x08 /* Private */,
      20,    0,  333,    2, 0x08 /* Private */,
      21,    0,  334,    2, 0x08 /* Private */,
      22,    0,  335,    2, 0x08 /* Private */,
      23,    0,  336,    2, 0x08 /* Private */,
      24,    1,  337,    2, 0x08 /* Private */,
      26,    1,  340,    2, 0x08 /* Private */,
      27,    0,  343,    2, 0x08 /* Private */,
      28,    1,  344,    2, 0x08 /* Private */,
      30,    1,  347,    2, 0x08 /* Private */,
      32,    0,  350,    2, 0x08 /* Private */,
      33,    0,  351,    2, 0x08 /* Private */,
      34,    0,  352,    2, 0x08 /* Private */,
      35,    0,  353,    2, 0x08 /* Private */,
      36,    0,  354,    2, 0x08 /* Private */,
      37,    0,  355,    2, 0x08 /* Private */,
      38,    0,  356,    2, 0x08 /* Private */,
      39,    0,  357,    2, 0x08 /* Private */,
      40,    0,  358,    2, 0x08 /* Private */,
      41,    0,  359,    2, 0x08 /* Private */,
      42,    0,  360,    2, 0x08 /* Private */,
      43,    0,  361,    2, 0x08 /* Private */,
      44,    0,  362,    2, 0x08 /* Private */,
      45,    0,  363,    2, 0x08 /* Private */,
      46,    0,  364,    2, 0x08 /* Private */,
      47,    0,  365,    2, 0x08 /* Private */,
      48,    0,  366,    2, 0x08 /* Private */,
      49,    0,  367,    2, 0x08 /* Private */,
      50,    0,  368,    2, 0x08 /* Private */,
      51,    0,  369,    2, 0x08 /* Private */,
      52,    0,  370,    2, 0x08 /* Private */,
      53,    0,  371,    2, 0x08 /* Private */,
      54,    0,  372,    2, 0x08 /* Private */,
      55,    0,  373,    2, 0x08 /* Private */,
      56,    0,  374,    2, 0x08 /* Private */,
      57,    0,  375,    2, 0x08 /* Private */,
      58,    0,  376,    2, 0x08 /* Private */,
      59,    2,  377,    2, 0x08 /* Private */,
      63,    1,  382,    2, 0x08 /* Private */,
      64,    1,  385,    2, 0x08 /* Private */,
      65,    1,  388,    2, 0x08 /* Private */,
      66,    0,  391,    2, 0x08 /* Private */,
      67,    1,  392,    2, 0x08 /* Private */,
      69,    0,  395,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QFont,   25,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   29,
    QMetaType::Void, QMetaType::Int,   31,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 61,   60,   62,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   68,
    QMetaType::Void,

       0        // eod
};

void Editor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Editor *_t = static_cast<Editor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->send_set_textindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->send_set_leftindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->send_set_rightindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->send_expand_edit_area((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->wyeditFindInBaseClicked(); break;
        case 5: _t->onUpdateIndentlineGeometrySlot(); break;
        case 6: _t->on_bold_clicked(); break;
        case 7: _t->on_italic_clicked(); break;
        case 8: _t->on_underline_clicked(); break;
        case 9: _t->on_monospace_clicked(); break;
        case 10: _t->on_code_clicked(); break;
        case 11: _t->on_clear_clicked(); break;
        case 12: _t->on_numericlist_clicked(); break;
        case 13: _t->on_dotlist_clicked(); break;
        case 14: _t->on_indentplus_clicked(); break;
        case 15: _t->on_indentminus_clicked(); break;
        case 16: _t->on_alignleft_clicked(); break;
        case 17: _t->on_aligncenter_clicked(); break;
        case 18: _t->on_alignright_clicked(); break;
        case 19: _t->on_alignwidth_clicked(); break;
        case 20: _t->on_fontselect_changed((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 21: _t->on_fontsize_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 22: _t->on_fontcolor_clicked(); break;
        case 23: _t->set_fontselect_on_display((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 24: _t->set_fontsize_on_display((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->on_showhtml_clicked(); break;
        case 26: _t->on_findtext_clicked(); break;
        case 27: _t->on_settings_clicked(); break;
        case 28: _t->on_showformatting_clicked(); break;
        case 29: _t->on_createtable_clicked(); break;
        case 30: _t->on_table_remove_row_clicked(); break;
        case 31: _t->on_table_remove_col_clicked(); break;
        case 32: _t->on_table_add_row_clicked(); break;
        case 33: _t->on_table_add_col_clicked(); break;
        case 34: _t->on_table_merge_cells_clicked(); break;
        case 35: _t->on_table_split_cell_clicked(); break;
        case 36: _t->on_insert_image_from_file_clicked(); break;
        case 37: _t->on_expand_edit_area_clicked(); break;
        case 38: _t->on_expand_tools_lines_clicked(); break;
        case 39: _t->on_save_clicked(); break;
        case 40: _t->on_back_clicked(); break;
        case 41: _t->on_find_in_base_clicked(); break;
        case 42: _t->on_show_text_clicked(); break;
        case 43: _t->on_to_attach_clicked(); break;
        case 44: _t->on_cursor_position_changed(); break;
        case 45: _t->on_selection_changed(); break;
        case 46: _t->on_undo(); break;
        case 47: _t->on_redo(); break;
        case 48: _t->on_cut(); break;
        case 49: _t->on_copy(); break;
        case 50: _t->on_paste(); break;
        case 51: _t->on_selectAll(); break;
        case 52: _t->on_findtext_signal_detect((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QTextDocument::FindFlags(*)>(_a[2]))); break;
        case 53: _t->on_indentline_change_textindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 54: _t->on_indentline_change_leftindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 55: _t->on_indentline_change_rightindent_pos((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 56: _t->on_indentline_mouse_release(); break;
        case 57: _t->on_customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 58: _t->on_context_menu_edit_image_properties(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Editor::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Editor::send_set_textindent_pos)) {
                *result = 0;
            }
        }
        {
            typedef void (Editor::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Editor::send_set_leftindent_pos)) {
                *result = 1;
            }
        }
        {
            typedef void (Editor::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Editor::send_set_rightindent_pos)) {
                *result = 2;
            }
        }
        {
            typedef void (Editor::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Editor::send_expand_edit_area)) {
                *result = 3;
            }
        }
        {
            typedef void (Editor::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Editor::wyeditFindInBaseClicked)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject Editor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Editor.data,
      qt_meta_data_Editor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Editor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Editor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Editor.stringdata0))
        return static_cast<void*>(const_cast< Editor*>(this));
    return QWidget::qt_metacast(_clname);
}

int Editor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 59)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 59;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 59)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 59;
    }
    return _id;
}

// SIGNAL 0
void Editor::send_set_textindent_pos(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Editor::send_set_leftindent_pos(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Editor::send_set_rightindent_pos(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Editor::send_expand_edit_area(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Editor::wyeditFindInBaseClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
