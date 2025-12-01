----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 11/30/2025 04:04:32 PM
-- Design Name: 
-- Module Name: scsa_tbh - Behavioral
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

entity scsa_tbh is
--  Port ( );
end scsa_tbh;

architecture Behavioral of scsa_tbh is

-- сигналы дл€ DUT
    signal CLK       : std_logic := '0';
    signal Reset     : std_logic := '0';
    signal En        : std_logic := '0';  -- в текущей реализации scsa_beh не используетс€
    signal DIn       : std_logic := '0';
    signal Load      : std_logic := '0';
    signal Seed      : std_logic_vector(0 to 2) := "000";
    signal Signature : std_logic_vector(0 to 2);

begin


    uut_scsa: entity work.scsa_beh
        port map (
            CLK       => CLK,
            Reset     => Reset,
            En        => En,
            DIn       => DIn,
            Load      => Load,
            Seed      => Seed,
            Signature => Signature
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
    begin
        En   <= '1'; 
        Load <= '0';
        Seed <= "000";


        Reset <= '1';
        wait for 20 ns;
        Reset <= '0';

        wait for 10 ns;


        -- бит 1
        DIn <= '1';
        wait until rising_edge(CLK);

        -- бит 0
        DIn <= '0';
        wait until rising_edge(CLK);

        -- бит 1
        DIn <= '1';
        wait until rising_edge(CLK);

        -- бит 1
        DIn <= '1';
        wait until rising_edge(CLK);

        -- чуть подождЄм, чтобы сигналы устаканились
        wait for 5 ns;

        ----------------------------------------------------------------
        -- ѕроверка итоговой сигнатуры
        -- ƒл€ начального состо€ни€ 000 и последовательности 1,0,1,1
        -- твой scsa_beh должен дать Signature = "111"
        ----------------------------------------------------------------
        assert Signature = "111"
            report "Unexpected signature value"
            severity error;

        ----------------------------------------------------------------
        --  онец моделировани€
        ----------------------------------------------------------------
        assert false
            report "SCSA simulation finished successfully"
            severity note;
        wait;
    end process;

end Behavioral;
