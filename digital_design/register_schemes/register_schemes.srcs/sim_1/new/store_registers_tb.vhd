----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/30/2025 03:22:08 PM
-- Design Name: 
-- Module Name: store_registers_tb - Behavioral
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

entity store_registers_tb is
--  Port ( );
end store_registers_tb;

architecture Behavioral of store_registers_tb is

-- общие входные сигналы
signal En    : std_logic := '0';
signal CLK   : std_logic := '0';
signal Reset : std_logic := '0';
signal DIn0  : std_logic := '0';
signal DIn1  : std_logic := '0';

-- выходы синхронных регистров
signal sync_beh_DOut0   : std_logic;
signal sync_beh_DOut1   : std_logic;
signal sync_struct_DOut0: std_logic;
signal sync_struct_DOut1: std_logic;

-- выходы асинхронных регистров
signal async_beh_DOut0   : std_logic;
signal async_beh_DOut1   : std_logic;
signal async_struct_DOut0: std_logic;
signal async_struct_DOut1: std_logic;

begin

    --------------------------------------------------------------------
    -- Инстанцирование устройств
    --------------------------------------------------------------------
    -- Синхронный поведенческий
    uut_sync_beh: entity work.store_register_beh
        port map (
            En    => En,
            CLK   => CLK,
            Reset => Reset,
            DIn0  => DIn0,
            DIn1  => DIn1,
            DOut0 => sync_beh_DOut0,
            DOut1 => sync_beh_DOut1
        );

    -- Синхронный структурный
    uut_sync_struct: entity work.store_register_struct
        port map (
            En    => En,
            CLK   => CLK,
            Reset => Reset,
            DIn0  => DIn0,
            DIn1  => DIn1,
            DOut0 => sync_struct_DOut0,
            DOut1 => sync_struct_DOut1
        );

    -- Асинхронный поведенческий
    uut_async_beh: entity work.store_register_async_beh
        port map (
            En    => En,
            Reset => Reset,
            DIn0  => DIn0,
            DIn1  => DIn1,
            DOut0 => async_beh_DOut0,
            DOut1 => async_beh_DOut1
        );

    -- Асинхронный структурный
    uut_async_struct: entity work.store_register_async_struct
        port map (
            En    => En,
            Reset => Reset,
            DIn0  => DIn0,
            DIn1  => DIn1,
            DOut0 => async_struct_DOut0,
            DOut1 => async_struct_DOut1
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
            begin
                ----------------------------------------------------------------
                -- Сброс
                ----------------------------------------------------------------
                Reset <= '1';
                En    <= '0';
                DIn0  <= '0';
                DIn1  <= '0';
                wait for 20 ns;          -- несколько тактов
        
                Reset <= '0';
                wait for 10 ns;
        
                -- после сброса выходы поведенческой и структурной версий
                -- (и синхронной, и асинхронной) должны совпадать
                assert (sync_beh_DOut0 = sync_struct_DOut0 and
                        sync_beh_DOut1 = sync_struct_DOut1)
                    report "Mismatch after reset (sync)" severity error;
        
                assert (async_beh_DOut0 = async_struct_DOut0 and
                        async_beh_DOut1 = async_struct_DOut1)
                    report "Mismatch after reset (async)" severity error;
        
                ----------------------------------------------------------------
                -- Запись значения при En='1'
                ----------------------------------------------------------------
                En   <= '1';
                DIn0 <= '1';
                DIn1 <= '0';
        
                -- асинхронный регистр обновится сразу, синхронный - по фронту CLK
                wait until rising_edge(CLK);
                wait for 1 ns;  -- время на распространение
        
                assert (sync_beh_DOut0 = sync_struct_DOut0 and
                        sync_beh_DOut1 = sync_struct_DOut1)
                    report "Mismatch after write 10 (sync)" severity error;
        
                assert (async_beh_DOut0 = async_struct_DOut0 and
                        async_beh_DOut1 = async_struct_DOut1)
                    report "Mismatch after write 10 (async)" severity error;
        
                ----------------------------------------------------------------
                -- Изменяем входы при En='0' (запрет записи)
                ----------------------------------------------------------------
                En   <= '0';
                DIn0 <= '0';
                DIn1 <= '1';
        
                wait until rising_edge(CLK);
                wait for 1 ns;
        
                assert (sync_beh_DOut0 = sync_struct_DOut0 and
                        sync_beh_DOut1 = sync_struct_DOut1)
                    report "Mismatch with En=0 (sync)" severity error;
        
                assert (async_beh_DOut0 = async_struct_DOut0 and
                        async_beh_DOut1 = async_struct_DOut1)
                    report "Mismatch with En=0 (async)" severity error;
        
                ----------------------------------------------------------------
                -- Ещё одно значение при En='1'
                ----------------------------------------------------------------
                En   <= '1';
                DIn0 <= '1';
                DIn1 <= '1';
        
                wait until rising_edge(CLK);
                wait for 1 ns;
        
                assert (sync_beh_DOut0 = sync_struct_DOut0 and
                        sync_beh_DOut1 = sync_struct_DOut1)
                    report "Mismatch after write 11 (sync)" severity error;
        
                assert (async_beh_DOut0 = async_struct_DOut0 and
                        async_beh_DOut1 = async_struct_DOut1)
                    report "Mismatch after write 11 (async)" severity error;
        
                ----------------------------------------------------------------
                -- Конец моделирования
                ----------------------------------------------------------------
                wait for 20 ns;
                assert false report "Simulation finished successfully" severity note;
                wait;
            end process;  

end Behavioral;
