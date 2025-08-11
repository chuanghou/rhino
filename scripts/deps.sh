#!/bin/bash

# 定义仓库地址列表（可以修改为你自己的仓库列表）
REPO_URLS=(
    "https://gitee.com/mirrors/googletest.git"
    "https://gitee.com/mirrors/capnproto.git"
)

# 临时目录（存放克隆的仓库）
WORK_DIR="./repos_tmp"
mkdir -p "$WORK_DIR"

# 遍历所有仓库地址
for REPO_URL in "${REPO_URLS[@]}"; do
    echo "==> Processing $REPO_URL"

    # 提取仓库名（如 https://github.com/user/repo.git → repo）
    REPO_NAME=$(basename "$REPO_URL" .git)

    # 进入工作目录
    cd "$WORK_DIR" || exit 1

    # 克隆仓库（默认只克隆 main/master 分支）
    git clone --quiet "$REPO_URL" "$REPO_NAME"
    cd "$REPO_NAME" || exit 1

    echo "  ✔ Cloned $REPO_NAME"

    # 获取所有远程分支并创建本地跟踪分支
    git fetch --all --quiet
    for REMOTE_BRANCH in $(git branch -r | grep -v '\->'); do
        LOCAL_BRANCH="${REMOTE_BRANCH#origin/}"
        # 如果本地分支不存在，则创建并跟踪远程分支
        if ! git show-ref --quiet "refs/heads/$LOCAL_BRANCH"; then
            git branch --quiet --track "$LOCAL_BRANCH" "$REMOTE_BRANCH"
            echo "  ✔ Created local branch: $LOCAL_BRANCH (tracking $REMOTE_BRANCH)"
        fi
    done

    # 返回上级目录
    cd ..

    # 打包整个仓库为 tar.gz
    tar -czf "${REPO_NAME}.tar.gz" "$REPO_NAME"
    echo "  ✔ Packed $REPO_NAME into ${REPO_NAME}.tar.gz"

    # 回到脚本初始目录
    cd ..
done

echo "==> All repositories processed. Check $WORK_DIR for .tar.gz files."
