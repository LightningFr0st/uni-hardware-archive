----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/30/2025 03:52:34 PM
-- Design Name: 
-- Module Name: lfsr_tbh - Behavioral
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

entity lfsr_tbh is
--  Port ( );
end lfsr_tbh;

architecture Behavioral of lfsr_tbh is

-- общие управляющие сигналы
signal CLK   : std_logic := '0';
signal Reset : std_logic := '0';
signal Load  : std_logic := '0';

-- один общий сид, потом разложим его по двум диапазонам
signal Seed_common : std_logic_vector(2 downto 0) := "001";

-- Seed для внутреннего генератора (0 to 2)
signal Seed_int : std_logic_vector(0 to 2);

-- выходы генераторов
signal DOut_ext : std_logic;
signal DOut_int : std_logic;

begin


    Seed_int(0) <= Seed_common(0);
    Seed_int(1) <= Seed_common(1);
    Seed_int(2) <= Seed_common(2);


    uut_ext: entity work.lfsr_external_beh
        port map (
            DOut  => DOut_ext,
            CLK   => CLK,
            Reset => Reset,
            Load  => Load,
            Seed  => Seed_common
        );

    uut_int: entity work.lfsr_internal_beh
        port map (
            DOut  => DOut_int,
            CLK   => CLK,
            Reset => Reset,
            Load  => Load,
            Seed  => Seed_int
        );


    clk_process : process
    begin
        while true loop
            CLK <= '0';
            wait for 5 ns;
            CLK <= '1';
            wait for 5 ns;
        end loop;
    end process;


    stim_proc : process
        variable i : integer;
    begin
        -- 1) Сброс
        Reset <= '1';
        Load  <= '0';
        wait for 20 ns;                -- пара тактов под сбросом

        -- 2) Отпускаем сброс, даём загрузку сидов
        Reset <= '0';
        Load  <= '1';
        wait until rising_edge(CLK);   -- на этом фронте оба LFSR загрузят Seed
        Load  <= '0';

        -- 3) Просто крутим несколько тактов и смотрим на DOut_ext / DOut_int
        for i in 0 to 15 loop
            wait until rising_edge(CLK);
            -- здесь можно поставить breakpoints и смотреть волны в симуляторе
        end loop;

        -- Конец моделирования
        wait for 20 ns;
        assert false
            report "LFSR simulation finished"
            severity note;
        wait;
    end process;

end Behavioral;
