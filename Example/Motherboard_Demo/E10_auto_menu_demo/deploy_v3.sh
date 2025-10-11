#!/bin/bash

# ����ű� - ���Զ��˵�ϵͳ����LS2K0300�忨���汾3��
echo "=== LS2K0300 �Զ��˵�ϵͳ����ű� v3.0 ==="

# ���ð忨IP��ַ
BOARD_IP="192.168.1.101"
BOARD_USER="root"

# ����Ҫ�ļ��Ƿ����
echo "����Ҫ�ļ�..."
if [ ! -f "user/main.cpp" ]; then
    echo "����: main.cpp �ļ�������!"
    exit 1
fi

if [ ! -f "user/CMakeLists.txt" ]; then
    echo "����: CMakeLists.txt �ļ�������!"
    exit 1
fi

if [ ! -f "user/cross.cmake" ]; then
    echo "����: cross.cmake �ļ�������!"
    exit 1
fi

if [ ! -f "user/auto_menu_v3.service" ]; then
    echo "����: auto_menu_v3.service �ļ�������!"
    exit 1
fi

# ������Ŀ
echo "������Ŀ..."
cd user
chmod +x build.sh
./build.sh

if [ $? -ne 0 ]; then
    echo "����ʧ��!"
    exit 1
fi

# ������ʱĿ¼
TEMP_DIR="/tmp/auto_menu_deploy"
mkdir -p $TEMP_DIR

# �������б�Ҫ�ļ�
echo "׼�������ļ�..."
cp E10_auto_menu_demo $TEMP_DIR/
cp auto_menu_v3.service $TEMP_DIR/
cp start_menu.sh $TEMP_DIR/
cp install_service.sh $TEMP_DIR/
cp ../README.md $TEMP_DIR/

# �����忨�ϵ�Ŀ¼
echo "�����忨Ŀ¼..."
ssh $BOARD_USER@$BOARD_IP "mkdir -p /home/root/E10_auto_menu_demo"

# �����ļ�
echo "�����ļ����忨..."
scp -r $TEMP_DIR/* $BOARD_USER@$BOARD_IP:/home/root/E10_auto_menu_demo/

# ����ִ��Ȩ��
echo "����ִ��Ȩ��..."
ssh $BOARD_USER@$BOARD_IP "chmod +x /home/root/E10_auto_menu_demo/E10_auto_menu_demo"
ssh $BOARD_USER@$BOARD_IP "chmod +x /home/root/E10_auto_menu_demo/start_menu.sh"
ssh $BOARD_USER@$BOARD_IP "chmod +x /home/root/E10_auto_menu_demo/install_service.sh"

# ֹͣ���з���
echo "ֹͣ���з���..."
ssh $BOARD_USER@$BOARD_IP "systemctl stop auto_menu"

# ��װ�·���
echo "��װ�·���..."
ssh $BOARD_USER@$BOARD_IP "cp /home/root/E10_auto_menu_demo/auto_menu_v3.service /etc/systemd/system/auto_menu.service"

# ��������
echo "��������..."
ssh $BOARD_USER@$BOARD_IP "systemctl daemon-reload"
ssh $BOARD_USER@$BOARD_IP "systemctl start auto_menu"
ssh $BOARD_USER@$BOARD_IP "systemctl enable auto_menu"

# ������״̬
echo "������״̬..."
ssh $BOARD_USER@$BOARD_IP "systemctl status auto_menu"

# ������ʱ�ļ�
rm -rf $TEMP_DIR

echo "=== ������� v3.0 ==="
echo "�忨IP: $BOARD_IP"
echo "�û���: $BOARD_USER"
echo "�˵������Ѱ�װ������Ϊ��������"
echo ""
echo "�°汾����:"
echo "1. ��Ŀ��ǰ̨���У����Խ���Ctrl+C�ź�"
echo "2. �˵�����Ŀ�������������ն˽���"
echo "3. �Ľ��˽��̹���ʹ��fork+exec���system"
echo ""
echo "ʹ�÷���:"
echo "1. �������Զ���ʾ�˵�"
echo "2. ʹ��KEY_3/KEY_2ѡ����Ŀ"
echo "3. ʹ��KEY_1������Ŀ��ǰ̨���У�"
echo "4. ʹ��Ctrl+Cֹͣ���е���Ŀ"
echo "5. ʹ��KEY_0���ز˵�"
echo "6. ʹ��KEY_3+KEY_2�˳��˵����ն�"
echo ""
echo "�����ֶ������˵���������:"
echo "ssh $BOARD_USER@$BOARD_IP"
echo "cd /home/root/E10_auto_menu_demo"
echo "./start_menu.sh"