----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/05/2025 04:19:32 PM
-- Design Name: 
-- Module Name: d_latch_tb - Behavioral
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

entity d_latch_tb is
--  Port ( );
end d_latch_tb;

architecture Behavioral of d_latch_tb is

signal D, E : STD_LOGIC := '0';
signal Q, nQ : STD_LOGIC;

component d_latch_beh is 
    Port ( D : in STD_LOGIC;
       E : in STD_LOGIC;
       Q : out STD_LOGIC;
       nQ : out STD_LOGIC);
end component;

component d_latch_struct is 
    Port ( D : in STD_LOGIC;
       E : in STD_LOGIC;
       Q : out STD_LOGIC;
       nQ : out STD_LOGIC);
end component;

begin
    DUT: d_latch_beh
        port map (
            D  => D,
            E  => E,
            Q  => Q,
            nQ => nQ
        );

    stim: process
    begin
        D <= '0'; E <= '0'; wait for 20 ns;

        E <= '1'; D <= '1'; wait for 20 ns;

        D <= '0'; wait for 20 ns;
        D <= '1'; wait for 20 ns;

        E <= '0'; D <= '0'; wait for 30 ns;

        E <= '1'; D <= '0'; wait for 20 ns;

        assert false report "Simulation finished" severity failure;
    end process;
end Behavioral;
