----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/30/2025 03:35:18 PM
-- Design Name: 
-- Module Name: johnson_tbh - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity johnson_tbh is
--  Port ( );
end johnson_tbh;

architecture Behavioral of johnson_tbh is

signal CLK    : std_logic := '0';
signal Reset  : std_logic := '0';

signal DOut0_beh  : std_logic;
signal DOut1_beh  : std_logic;
signal DOut0_str  : std_logic;
signal DOut1_str  : std_logic;

begin

--------------------------------------------------------------------
    -- Инстанцирование поведенческого счётчика Джонсона
    --------------------------------------------------------------------
    uut_beh: entity work.johnson_beh
        port map (
            CLK   => CLK,
            Reset => Reset,
            DOut0 => DOut0_beh,
            DOut1 => DOut1_beh
        );

    --------------------------------------------------------------------
    -- Инстанцирование структурного счётчика Джонсона
    --------------------------------------------------------------------
    uut_struct: entity work.johnson_struct
        port map (
            CLK   => CLK,
            Reset => Reset,
            DOut0 => DOut0_str,
            DOut1 => DOut1_str
        );

    --------------------------------------------------------------------
    -- Генерация тактового сигнала
    --------------------------------------------------------------------
    clk_process : process
    begin
        while true loop
            CLK <= '0';
            wait for 5 ns;
            CLK <= '1';
            wait for 5 ns;
        end loop;
    end process;

    --------------------------------------------------------------------
    -- Стимулы и проверки
    --------------------------------------------------------------------
    stim_proc : process
        variable i : integer;
    begin
        -- начальный сброс
        Reset <= '1';
        wait for 20 ns;              -- пара тактов с активным сбросом
        Reset <= '0';

        -- прогоним несколько тактов и на каждом сравним выходы
        for i in 0 to 15 loop
            wait until rising_edge(CLK);
            wait for 1 ns;           -- время на распространение сигналов

            -- сравнение поведенческой и структурной реализации
            assert (DOut0_beh = DOut0_str and
                    DOut1_beh = DOut1_str)
                report "Mismatch between johnson_beh and johnson_struct at cycle "
                       & integer'image(i)
                severity error;
        end loop;

        -- если дошли сюда без ошибок - всё ок
        assert false
            report "Johnson counter simulation finished successfully"
            severity note;
        wait;
    end process;


end Behavioral;
