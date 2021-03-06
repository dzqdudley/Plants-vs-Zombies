#include"Game.h"
#include<stdlib.h>
#include<time.h>
//bool garden_refresh_need = true;
bool store_refresh_need = true;
Garden Game::garden;
Store Game::store;
Game:: Game()
{
    state = normal;
    cur_row = 0;
    cur_col = 1;
    add_zombie_count = clock();
    add_zombie_speed = 6000;
}
void Game::delete_zombie(int row, int col, Zombie* zombie)//ɾ��Zombie
{
    garden.cells[row][col].delete_zombie(zombie);
}
void Game::zombie_dead()
{
    for (auto i = all_zombies.begin(); i != all_zombies.end();)
    {
        if ((*i)->cur_hp <= 0)
        {
            delete_zombie((*i)->get_row(), (*i)->get_col(), (*i));
            all_zombies.erase(i);
            break;
        }
        else i++;
    }
}
void Game::display_bullets()
{
    if (all_bullets.size())
    {
        for (int i = 0; i < (int)all_bullets.size(); i++)//��ʾ�ӵ�
                all_bullets[i]->display();
    }
}
//��ȫˢ����Ļ
void Game::refresh_everything()
{
    garden.refresh(cur_row, cur_col);
    if (store_refresh_need)
    {
        store.display(cur_row, cur_col);
        store_refresh_need = false;
    }
}
//�̵�������
bool Game::goto_store()
{
    char ch;
        if (_kbhit())//�����а�������
        {
             ch = _getch();//��ȡ�ַ�
            switch (ch)
            {
            case 'b':case 'B'://������Ʒ
                if ((store.merchandises[cur_col - 1].get_count() == 0 ||
                    ((int)clock() - (int)store.merchandises[cur_col - 1].get_count()) >
                    (int)store.merchandises[cur_col - 1].get_speed())
                    && store.sunshine >= store.merchandises[cur_col - 1].get_price())
                {
                    store.purchase(store.merchandises[cur_col - 1].get_good_name());
                    state = placing;
                }
                break;
            case 'w':case 'W': case 72://�����ƶ�
                if (cur_row > 0)
                    cur_row--;
                break;
            case 's':case 'S':case 80://�����ƶ�
                cur_row++;
                store_refresh_need = true;
                break;
            case 'a':case 'A':case 75://�����ƶ�
                if (cur_col > 1)
                    cur_col--;
                store_refresh_need = true;
                break;
            case 'd':case 'D':case 77://�����ƶ�
                if (cur_col < (int)store.merchandises.size())
                    cur_col++;
                store_refresh_need = true;
                break;
            case '8'://��ͣ
                pause();
                break;
            case 'q':case'Q':
                return false;
            default:
                break;
            }
            garden.cells[cur_row][cur_col].set_refresh_need(true);
        }
        if (state == shopping && !(cur_row == 0 && cur_col <= (int)store.merchandises.size()))
            state = normal;
        return true;
}
//����ֲ��
bool Game::place_plant()
{
        if (_kbhit())//�����а�������
        {
            char ch = _getch();//��ȡ�ַ�
            garden.cells[cur_row][cur_col].set_refresh_need(true);
            switch (ch)
            {
            case 'x':case 'X'://ȡ������
                state = normal;
                return true;
            case 13://�س�������
                int i;
                for (i = 0; i < (int)store.merchandises.size(); i++)
                    if (store.merchandises[i].if_selected())
                        break;
                if (cur_row > 0 && (garden.cells[cur_row][cur_col].has_plant() == false || store.merchandises[i].get_good_name() == "pumpkin"))
                {
                    store.sunshine = store.sunshine - store.merchandises[i].get_price();
                    store.merchandises[i].set_selected(false);
                    store.merchandises[i].set_count(clock());
                    garden.cells[cur_row][cur_col].add_plant(store.merchandises[i].get_good_name());
                    if (garden.cells[cur_row][cur_col].has_zombie())//���ŵؿ���Zombie
                    {
                        garden.cells[cur_row][cur_col + 1].set_refresh_need(true);
                        set_coord((cell_length)*(cur_col+1), (cell_height)*cur_row+3);
                        cout << ' ';
                        set_coord((cell_length) * (cur_col + 1), (cell_height)*cur_row + 4);
                        cout << ' ';
                    }
                    state = normal;
                    break;
                    refresh_everything();
                }
                break;
            case 'w':case 'W':case 72://�����ƶ�
                if (cur_row > 1)
                    cur_row--;
                store_refresh_need = true;
                break;
            case 's':case 'S':case 80://�����ƶ�
                if (cur_row < max_row - 1)
                    cur_row++;
                store_refresh_need = true;
                break;
            case 'A':case 'a':case 75://�����ƶ�
                if ((cur_row==0&&cur_col>1)||cur_row&&cur_col > 0)
                    cur_col--;
                store_refresh_need = true;
                break;
            case 'd':case 'D':case 77://�����ƶ�
                if ((cur_row==0&&cur_col < (int)store.merchandises.size())||cur_row&&cur_col < max_col - 2)
                    cur_col++;
                store_refresh_need = true;
                break;
            case '8'://��ͣ
                pause();
                break;
            case 'q':case'Q':
                return false;
            default:break;
            }
            garden.cells[cur_row][cur_col].set_refresh_need(true);
            refresh_everything();
        }
        return true;
}
void Game::loop()
{
    add_zombie(rand()%(max_row+1), max_col - 1);//����λ������Zombie
    add_zombie_count = clock();
    garden.init();
    garden.display();
    while (1)
    {
        refresh_everything();
        if (check_game_status())
            break;
        if ((int)(clock()-add_zombie_count) > add_zombie_speed)
        {

            add_zombie(rand() % max_row, max_col - 1);//����λ������Zombie
            add_zombie_count = clock();
            if(add_zombie_speed>4500)//�ӿ�����Zombie���ٶ�
                add_zombie_speed -= 300;
        }
        all_bullets_move();
        display_bullets();
        all_zombies_move();
        basketball_hit_plant();
        bullet_hit_zombie();
        plant_eaten();
        zombie_dead();
        function();
        store.natural_sunshine();
        if (state!=placing && cur_row == 0 && cur_col < (int)store.merchandises.size())//�������̵���
            state = shopping;
        if (state == shopping)
        {
            if (!goto_store())
            {
                end_game();
                return;
            }
        }
        else if (state == placing)
        {
            if (!place_plant())
            {
                end_game();
                return;
            }
        }
        else if (state == normal)
        {
            if (_kbhit())//�����а�������
            {
                char ch = _getch();//��ȡ�ַ�
                garden.cells[cur_row][cur_col].set_refresh_need(true);
                switch (ch)
                {
                case 'w':case 'W':case 72://�����ƶ�
                    if (cur_row > 1||(cur_row==1&&cur_col>0&& cur_col<=(int)store.merchandises.size()))
                        cur_row--;
                    if (cur_row == 0)
                    {
                        store_refresh_need = true;
                        state = shopping;
                    }
                    break;
                case 's':case 'S':case 80://�����ƶ�
                    if (cur_row < max_row - 1)
                        cur_row++;
                    break;
                case 'a':case 'A':case 75://�����ƶ�
                    if (cur_col > 0)
                        cur_col--;
                    break;
                case 'd':case 'D':case 77://�����ƶ�
                    if (cur_col < max_col - 2)
                        cur_col++;
                    break;
                case '8'://��ͣ
                    pause();
                    break;
                case 'q':case'Q':
                    end_game();
                    return;
                default:break;
                }
                garden.cells[cur_row][cur_col].set_refresh_need(true);
            }
        }
    }
}
//ָ��λ�ô�����Zombie
void Game::add_zombie(int row, int col)
{
    if (row == 0)
        row = 1;
    else if (row > 4)
        row = 4;
    garden.cells[row][col].set_has_zombie(true);
    Zombie* zombie = NULL;
    int type = rand() % 6;
    switch (type)
    {
    case 0:
        zombie= new Zombie;
        break;
    case 1:
        zombie = new Conehead_Zombie;
        break;
    case 2:
        zombie = new Newspaper_Zombie;
        break;
    case 3:
        zombie = new Catapult_Zombie;
        break;
    case 4:
        zombie = new Jack_Zombie;
        break;
    case 5:
        zombie = new Pole_Zombie;
        break;
    default:
        zombie = new Zombie;
        break;
    }
    zombie->set_loc(col * cell_length, row * cell_height, row, col);
    garden.cells[row][col].push_back_zombie(zombie);
    all_zombies.push_back(zombie);
    garden.cells[row][col].set_refresh_need(true);
    return;
}
void Game::function()
{
    garden.plant_function(*this);
    /*for (int i = 1; i < max_row; i++)
        for (int j = 0; j < max_col - 1; j++)
            if (garden.cells[i][j].has_plant())
                garden.cells[i][j].plant_function(*this);*/
}
void Game::all_zombies_move()
{
    for (auto i = all_zombies.begin(); i != all_zombies.end(); i++)
    {
        int temp = (*i)->get_col();
        if ((*i)->move())
        {
            garden.cells[(*i)->get_row()][(*i)->get_col()].set_refresh_need(true);
        }
        if ((*i)->get_x() < 0)
            end_game();
        if (temp != (*i)->get_col())//Zombie�ƶ�����һ�ؿ�
        {
            garden.cells[(*i)->get_row()][(*i)->get_col()].set_has_zombie(true);
            garden.cells[(*i)->get_row()][(*i)->get_col()].push_back_zombie((*i));
            delete_zombie((*i)->get_row(),temp,(*i));//��ǰ�ؿ�ɾ���뿪�˵�Zombie
            garden.cells[(*i)->get_row()][temp].set_refresh_need(true);
        }
        if (garden.cells[(*i)->get_row()][(*i)->get_col()].has_plant() || garden.cells[(*i)->row][(*i)->col].if_pumpkin())//�ؿ�����ֲ��
        {
            (*i)->is_blocked = true;
            if ((*i)->get_name() == "Pole_Zombie")
            {
                if (garden.cells[(*i)->get_row()][(*i)->get_col()].has_plant() && garden.get_cell_plant_name((*i)->row,(*i)->col) == "Tallnut")
                    (*i)->skippable = false;
                else
                {
                    (*i)->skippable = true;
                    (*i)->is_blocked = true;
                }
            }
        }
        if ((*i)->explode)
        {
            garden.set_explode((*i)->get_row(), (*i)->get_col());
            delete_zombie((*i)->get_row(), (*i)->get_col(), (*i));//��ǰ�ؿ�ɾ��Zombie
        }
    }
}

void Game::plant_eaten()
{
    for (int i = 1; i < max_row; i++)
        for (int j = 0; j < max_col - 1; j++)
            if ((garden.cells[i][j].if_pumpkin() || garden.cells[i][j].has_plant()) && garden.cells[i][j].has_zombie())
                garden.cells[i][j].plant_eaten();
}

void Game::bullet_hit_zombie()
{
    for (auto j = all_bullets.begin(); j != all_bullets.end();)
        if (garden.cells[(*j)->get_row()][(*j)->get_col()].get_zombies_size())
        {
            auto i = garden.cells[(*j)->get_row()][(*j)->get_col()].get_first_zombie();//ֻ���˺�һ��Zombie
            if ((*j)->get_col() >= max_col)//������Χ
            {
                j = all_bullets.erase(j);
            }
            else if ((*j)->get_row() == (*i)->get_row() && (*j)->get_col() == (*i)->get_col())//����
            {
                (*i)->get_hit((*j)->get_attack());
                if ((*j)->get_type() == "Snow")
                    (*i)->set_slowed();
                garden.cells[(*i)->row][(*i)->col].set_refresh_need(true);
                set_coord(((*j)->get_col()) * length, (*j)->get_y());
                std::cout << '#';
                if((*j)->get_col()<max_col-1)
                {
                set_coord(((*j)->get_col()+1) * length, (*j)->get_y());
                std::cout << '#';
                }
                j = all_bullets.erase(j);
            }
            else j++;
        }
        else
            j++;
}
bool Game::check_game_status()
{
    for (auto i = all_zombies.begin(); i != all_zombies.end(); i++)
        if (((*i)->get_col() == 0 && !garden.cells[(*i)->get_row()][0].has_plant()) || ((*i)->get_col() == 0 && (*i)->get_name() == "Pole_Zombie" && (*i)->is_pole()))
        {
            system("cls");
            set_coord((cell_length)*max_col/2, (cell_height) * max_row/2);
            color_cout("Game Over!",Zombie_color);
            set_coord((cell_length)*max_col / 2, (cell_height)*max_row / 2+2);
            color_cout("Your Score is ", Zombie_color);
            color_cout(store.score, Zombie_color);
            set_coord((cell_length)*max_col / 2, (cell_height)*max_row / 2 + 4);
            system("pause");
            return true;
        }
    return false;
}
void Game::all_bullets_move()
{
    for (int i = 0; i < (int)all_bullets.size(); i++)//�ӵ��ƶ�
    {
        if (all_bullets[i]->get_col() >= max_col)//������Χ
            continue;
        int temp = all_bullets[i]->get_col();
        if (all_bullets[i]->move(garden.cells[all_bullets[i]->get_row()][temp].has_plant()))
        {
            if (temp != all_bullets[i]->get_col())//������
                garden.cells[all_bullets[i]->get_row()][temp].set_refresh_need(true);
            if(garden.cells[all_bullets[i]->get_row()][all_bullets[i]->get_col()].has_plant())
                garden.cells[all_bullets[i]->get_row()][all_bullets[i]->get_col()].set_refresh_need(true);
        }
    }
}
void Game::pause()
{
    int cur_state = state;
    clock_t cur_time = clock();
    while (1)
    {
        char ch;
        if (_kbhit())
        {
            ch = _getch();
            switch (ch)
            {
            case 'p':case 'P'://��ͣ����
                state = cur_state;
                store.natural_sun_cnt += clock() - cur_time;
                add_zombie_count += clock() - cur_time;
                return;
            default:;
            }
        }
    }
}
void Game::end_game()
{
    system("cls");
    set_coord((cell_length) * max_col / 2, (cell_height)*max_row / 2);
    color_cout("Game Over!", Zombie_color);
    set_coord((cell_length)*max_col / 2, (cell_height)*max_row / 2 + 2);
    color_cout("Your Score is ", Zombie_color);
    color_cout(store.score, Zombie_color);
    set_coord((cell_length)*max_col / 2, (cell_height)*max_row / 2 + 4);
    system("pause");
}
void Game::basketball_hit_plant()
{
    int ball_atk = 0;
    for (auto i = all_zombies.begin(); i != all_zombies.end(); i++)
    {
        if ((*i)->get_attack() == 5000 /*&& (*i)->get_ball_cnt() > 0*/)//Catapult_Zombie
        {
            int j;
            for (j = 0; j < max_col - 1; j++)
                if (garden.cells[(*i)->get_row()][j].has_plant())
                {
                    (*i)->plant_infront = true;
                    ball_atk = (*i)->get_ball_attack();
                    if (ball_atk != 0)
                    {
                        garden.plant_minus_hp((*i)->get_row(),j,ball_atk);
                        (*i)->decrease_cnt();
                        garden.cells[(*i)->get_row()][j].set_refresh_need(true);
                        if (garden.get_cur_hp((*i)->get_row(),j) <= 0)
                            garden.cells[(*i)->get_row()][j].delete_plant();
                    }
                    break;
                }
            if (j == max_col - 1)
                (*i)->plant_infront = false;
        }
    }
}
//������ͨ�ӵ�
bool Game::add_ordinary_bullet(int row, int col)
{
        for (auto i = all_zombies.begin(); i != all_zombies.end(); i++)
        {
            if ((*i)->get_row() == row)
            {
                Bullet* bullet = new Bullet;
                bullet->set_loc(col * cell_length + cell_length + 1, (row + 1) * (cell_height + 1) - 3, row, col + 1);
                all_bullets.push_back(bullet);
                return true;
            }
        }
        return false;
}
//���Ӻ����ӵ�
bool Game::add_snow_bullet(int row, int col)
{
    for (auto i = all_zombies.begin(); i != all_zombies.end(); i++)
    {
        if ((*i)->get_row() == row)
        {
            Bullet* bullet = new SnowBullet;
            bullet->set_loc(col * cell_length + cell_length + 1, (row + 1) * (cell_height + 1) - 3, row, col + 1);
            all_bullets.push_back(bullet);
            return true;
        }
    }
    return false;
}
void Game::squash_function(int row, int col)
{
    for (auto i = all_zombies.begin(); i != all_zombies.end(); i++)
    {
        if ((*i)->get_row() == row && (*i)->get_col() == col)
        {
            cell_delete_zombie(row, col, *i);
            (*i)->set_cur_hp(0);
        }
    }
}
void Game::cherrybomb_function(int row, int col)
{
    for (auto i = all_zombies.begin(); i != all_zombies.end(); i++)
    {
        if ((*i)->get_row() == row && (*i)->get_col() == col)
        {
            cell_delete_zombie(row, col, *i);
            (*i)->set_cur_hp(0);
        }
    }
}
void Game::garlic_function(int row, int col)
{
    for (auto i = all_zombies.begin(); i != all_zombies.end(); i++)
    {
        if ((*i)->get_row() == row && (*i)->get_col() == col)
        {
            if ((*i)->get_name() == "Pole_Zombie" && (*i)->is_pole())
                continue;
            cell_set_refresh_need((*i)->get_row(), (*i)->get_col(), true);
            delete_zombie(row, col, *i);
            if ((*i)->get_row() < max_row - 1)
            {
                (*i)->set_row((*i)->get_row() + 1);
                (*i)->set_y((*i)->get_y() + cell_height);
            }
            else
            {
                (*i)->set_row((*i)->get_row() - 1);
                (*i)->set_y((*i)->get_y() - cell_height);
            }
            (*i)->set_is_blocked(false);
            cell_set_set_has_zombie((*i)->get_row(), (*i)->get_col(), true);
            cell_set_refresh_need((*i)->get_row(), (*i)->get_col(), true);
            cell_push_back_zombie((*i)->get_row(), (*i)->get_col(), (*i));
            break;
        }
    }
}
