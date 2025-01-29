# renameProject.py
GitHubのテンプレート機能を使って新しいプロジェクトを作成した後に，一度実行してください

## 動作の概要
- .projectファイル内のプロジェクト名を変更します
- .cprojectファイル内のプロジェクト名を変更します
- *.iocファイルの名前を変更します
- *.launchファイル，Debugディレクトリ，Releaseディレクトリを削除します

[参考文献](https://tekuteku-embedded.xyz/2023/03/15/stm32cubeide-copy/)

## 使い方
```bash
python renameProject.py
```

# before.py
*.iocファイルを編集して，CubeIDEでコード生成する前に実行されるようにします．\
このリポジトリでは既に設定済みです．

# after.py
*.iocファイルを編集して，CubeIDEでコード生成した後に実行されるようにします．\
このリポジトリでは既に設定済みです．
