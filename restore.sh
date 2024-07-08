#!/bin/bash

# 定义目录变量
SOURCE_DIR="linux-0.11"
BACKUP_DIR="linux-0.11-back"
MODIFIED_DIR="modified-files"

# 忽略的正则表达式数组
IGNORE_PATTERNS=(
    '^\.(git|hg|svn)$'  # 忽略版本控制目录
    '.*\.o$'  # 忽略编译生成的目标文件
    '.*\.a$' # 忽略编译生成的静态库文件
    'System.map'  # 忽略 System.map 文件
    'bootsect'  # 忽略 bootsect 文件
    'setup'  # 忽略 setup 文件
    'keyboard.*\.s'  # 忽略键盘相关汇编文件
    'Image'  # 忽略 Image 文件
    'tool'  # 忽略 tool 目录
    )

# 解析参数
COPY_MODIFIED=false
RESTORE_ORIGINAL=false
COPY_MODIFIED_TO_SOURCE=false  # [新增] 标记是否将修改后的文件复制回 SOURCE_DIR

while getopts "crb" opt; do  # [修改] 增加 b 参数
    case $opt in
        c)
            COPY_MODIFIED=true
            ;;
        r)
            RESTORE_ORIGINAL=true
            ;;
        b)
            COPY_MODIFIED_TO_SOURCE=true  # [新增] 处理 b 参数
            ;;
        *)
            echo "Usage: $0 [-c] [-r] [-b]"
            echo "  -c  Copy modified files to target directory"
            echo "  -r  Restore original files from backup"
            echo "  -b  Copy modified files back to source directory"
            exit 1
            ;;
    esac
done

# 检查参数冲突
if $COPY_MODIFIED && $COPY_MODIFIED_TO_SOURCE; then  # [新增] 参数冲突检查
    echo "Error: -c and -b options cannot be used together."
    exit 1
fi

# 创建存放修改过文件的目标目录（如果需要复制）
if $COPY_MODIFIED; then
    mkdir -p "$MODIFIED_DIR"
fi

# 检查文件是否应被忽略
should_ignore() {  # [新增] 检查文件是否应被忽略的函数
    local file=$1
    for pattern in "${IGNORE_PATTERNS[@]}"; do  # [新增] 循环遍历忽略模式
        if [[ $file =~ $pattern ]]; then
            # 打印被忽略的文件名
            echo "Ignored: $file"
            return 0
        fi
    done
    return 1
}

# 遍历原始目录中的文件
find "$SOURCE_DIR" -type f | while read -r FILE; do
    # 获取相对于原始目录的相对路径
    REL_PATH="${FILE#$SOURCE_DIR/}"
    
    # 检查文件是否应被忽略
    if should_ignore "$REL_PATH"; then  # [新增] 调用 should_ignore 函数检查文件是否应被忽略
        continue
    fi

    # 获取相应的备份文件路径
    BACKUP_FILE="$BACKUP_DIR/$REL_PATH"
    
    # 检查备份文件是否存在
    if [ -e "$BACKUP_FILE" ]; then
        # 检查文件是否被修改
        if ! cmp -s "$FILE" "$BACKUP_FILE"; then
            echo "Modified: $REL_PATH"
            if $COPY_MODIFIED; then
                # 文件被修改，复制到目标目录
                mkdir -p "$MODIFIED_DIR/$(dirname "$REL_PATH")"
                cp "$FILE" "$MODIFIED_DIR/$REL_PATH"
            fi
            if $RESTORE_ORIGINAL; then
                # 恢复原文件
                cp "$BACKUP_FILE" "$FILE"
            fi
        fi
    else
        # 将新增文件复制到目标目录
        if $COPY_MODIFIED; then
            mkdir -p "$MODIFIED_DIR/$(dirname "$REL_PATH")"
            cp "$FILE" "$MODIFIED_DIR/$REL_PATH"
        fi
        if $RESTORE_ORIGINAL; then
            # 删除原文件
            rm -f "$FILE"
        fi
    fi 

done

# [新增] 将 MODIFIED_DIR 中的文件复制回 SOURCE_DIR
if $COPY_MODIFIED_TO_SOURCE; then
    find "$MODIFIED_DIR" -type f | while read -r FILE; do
        REL_PATH="${FILE#$MODIFIED_DIR/}"
        mkdir -p "$SOURCE_DIR/$(dirname "$REL_PATH")"
        cp "$FILE" "$SOURCE_DIR/$REL_PATH"
    done
    echo "修改后的文件已复制回源目录 $SOURCE_DIR。"
fi

echo "脚本执行完毕。"
if $COPY_MODIFIED; then
    echo "修改过的文件已复制到 $MODIFIED_DIR。"
fi
if $RESTORE_ORIGINAL; then
    echo "原文件已恢复到初始状态。"
fi